#include "LinkerCall.h"
#include "LinkerDiagnostic.h"
#include "LogManager.h"

#include <QProcess>
#include <QTextStream>

LinkerCall::LinkerCall(const QStringList& objectFiles, const QFileInfo& executablePath, QObject *parent)
	: ToolCall(parent)
	, objectFiles(objectFiles)
	, executablePath(executablePath)
	, errorCount(0)
	, warningCount(0)
{
}

LinkerCall::~LinkerCall()
{
}

void LinkerCall::start()
{
	// Ensambles a command, something such as
	// c++ -Wall -std=c++11 /path/player/unit/main.o /path/player/unit/MyClass.o -o /path/player/unit/unit
	QStringList arguments( getDefaultCompilerArguments() );
	arguments << getDefaultLinkerArguments();
	arguments << objectFiles;
	arguments << "-o" << executablePath.absoluteFilePath();

	// Call the linker
	Q_ASSERT(process == nullptr);
	process = new QProcess(this);

	// When linking process finishes, call a method of this class
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished()));

	// Start the process
	process->start(getCxxCompiler(), arguments);
	qCInfo(logBuild) << process->program() << process->arguments().join(" ");
	state = ToolCallState::started;
}

void LinkerCall::processFinished()
{
	qCInfo(logBuild) << "Linking finished with exit code" << process->exitCode() << "and exit status" << process->exitStatus();

	// Compilers does not send data to the standard output, but if there are something, print it
	const QString& compilerStandarOutput = process->readAllStandardOutput();
	if (compilerStandarOutput.length() > 0 )
		qCDebug(logBuild) << "stdout [" << compilerStandarOutput << "]";

	QTextStream errorOutput( process->readAllStandardError() );
	while ( ! errorOutput.atEnd() )
	{
		// Get the line
		const QString& line = errorOutput.readLine();

		// Ignore the exit status output of the linker
		if ( line.contains("exit status") )
			continue;

		// Create a diagnostic for this line
		LinkerDiagnostic* diagnostic = new LinkerDiagnostic(line);
		diagnostics.append( diagnostic );

		// If the diagnostic is a normal or fatal error, increase its number
		if ( diagnostic->isError() )
			++errorCount;
		else
			++warningCount;
	}

	// All the linker output was processed. The linker call is finished
	state = ToolCallState::finished;
	emit finished();
}
