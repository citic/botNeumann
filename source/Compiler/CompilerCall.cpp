#include "CompilerCall.h"
#include "Diagnostic.h"
#include <QDir>
#include <QProcess>
#include <QRegularExpression>
#include <QTextStream>

CompilerCall::CompilerCall(const QFileInfo& sourcePath, QObject* parent)
	: ToolCall(parent)
	, sourcePath(sourcePath)
	, targetPath(getTargetPathFor(sourcePath))
	, errorCount(0)
	, warningCount(0)
{
	updateState();
}

CompilerCall::~CompilerCall()
{
}

void CompilerCall::updateState()
{
	// If there exists the object file, and it is updated avoid to compile it again
	if ( targetPath.exists() && isFileNewerThan(targetPath, sourcePath) )
		state = ToolCallState::finished; // No compilation required
	else
		state = ToolCallState::scheduled; // Schedule this file for compilation
}

QFileInfo CompilerCall::getTargetPathFor(const QFileInfo& sourceFilePath)
{
	return sourceFilePath.absolutePath() + QDir::separator() + sourceFilePath.completeBaseName() + ".o";
}

void CompilerCall::clear()
{
	for (int i = 0; i < diagnostics.size(); ++i)
		delete diagnostics[i];

	diagnostics.clear();
}

#include <QDebug>

void CompilerCall::compile()
{
	// Ensambles a command, something such as
	// c++ -Wall -std=c++11 -c /path/player/unit/main.cpp -o /path/player/unit/main.o
	QStringList arguments( getDefaultCompilerArguments() );
	arguments << "-c" << sourcePath.absoluteFilePath();

	// The object file is the same source chaging its extension to .o
	arguments << "-o" << targetPath.absoluteFilePath();

	// Call the compiler in another process
	Q_ASSERT(process == nullptr);
	process = new QProcess(this);

	// When compilation finishes, call a method of this class
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished()));

	// Start the process
	process->start(getCxxCompiler(), arguments);
	qDebug() << process->program() << process->arguments().join(" ");
	state = ToolCallState::started;
}

void CompilerCall::processFinished()
{
	qDebug() << "Compilation finished with exit code" << process->exitCode() << "and exit status" << process->exitStatus();

	// Compilers does not send data to the standard output, but if there are something, print it
	const QString& compilerStandarOutput = process->readAllStandardOutput();
	if (compilerStandarOutput.length() > 0 )
		qDebug() << "stdout [" << compilerStandarOutput << "]";

	// Expression to know if a line starts a new error/warning:
	// "/path/to/sourcefile.cpp:line:col: severity: short diagnostic message"
	QRegularExpression diagnosticStartLine("^(\\w?:?[^:]+):(\\d+):(\\d+): (\\w+): (.+)$");

	// Each time a diagnostic start line is found, a new diagnostic object is created
	// This pointer refers to the last one, the active one
	Diagnostic* currentDiagnostic = nullptr;

	// Warnings and errors are sent to the standard error by compilers, not standard output
	QTextStream errorOutput( process->readAllStandardError() );
	while ( ! errorOutput.atEnd() )
	{
		// Get a line and compare it against the pattern to find the start of a diagnostic
		const QString& line = errorOutput.readLine();
		QRegularExpressionMatch match = diagnosticStartLine.match(line);

		// If the line matches the begin of a diagnostic, create one and read it
		if ( match.hasMatch() )
		{
			// Create the new diagnostic and extract its parts from the regular expression match
			currentDiagnostic = new Diagnostic(match);
			diagnostics.append(currentDiagnostic);

			// If the diagnostic is a normal or fatal error, increase its number
			if ( currentDiagnostic->isError() )
				++errorCount;
			else
				++warningCount;
		}
		// If the line is not the begin of a diagnostic, append the line to the active diagnostic
		else if ( currentDiagnostic )
			currentDiagnostic->appendExplanation(line);
		// Some lines may be ignored
		else
			qDebug() << "ignored stderr line {" << line << "}";
	}

	// All the compiler output was processed. This source file has finished its compiling process
	state = ToolCallState::finished;
	emit finished();
}
