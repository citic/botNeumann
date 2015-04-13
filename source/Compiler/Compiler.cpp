#include "CompilerCall.h"
#include "Compiler.h"
#include "Diagnostic.h"
#include <QStringList>

Compiler::Compiler(QObject *parent)
	: QObject(parent)
	, errorCount(-1)
	, warningCount(-1)
	, currentCompilerCall(-1)
{
}

Compiler::~Compiler()
{
	clear();
}

void Compiler::clear()
{
	for (int i = 0; i < compilerCalls.size(); ++i)
		delete compilerCalls[i];

	compilerCalls.clear();
	objectFiles.clear();
}

void Compiler::compile(const QFileInfoList& filepaths, const QFileInfo& executablePath)
{
	// Clear results of previous compilation process
	clear();

	// Copy the executable file path
	this->executablePath = executablePath;

	// If there is not executable, it must be linked/created
	shouldLinkExecutable = ! executablePath.exists();

	// Fill the list of source files that require be compiled
	scheduleCompilerCalls(filepaths);

	// Reset the number of warnings and errors found
	errorCount = warningCount = 0;

	// Start to compile the first scheduled unit
	currentCompilerCall = 0;
	compileNextSourceFile();
}

void Compiler::scheduleCompilerCalls(const QFileInfoList& filepaths)
{
	// For each file, schedule these that require generate its object file
	foreach ( QFileInfo fileInfo, filepaths )
	{
		// Analyze if this source file must be compiled
		CompilerCall* compilerCall = new CompilerCall(fileInfo);

		// Regardless this file should be compiled or not, it must be included in the linking phase
		objectFiles.append( compilerCall->getTargetPath().absoluteFilePath() );

		// If this .cpp file, it is is newer than the executable, the executable must be updated
		if ( shouldLinkExecutable == false && compilerCall->isTargetNewerThan(executablePath) )
			shouldLinkExecutable = true;

		// If the file must be compiled, schedule it, else, remove it
		if ( compilerCall->getState() != CompilationState::finished )
			compilerCalls.append(compilerCall);
		else
			delete compilerCall;
	}
}

void Compiler::compileNextSourceFile()
{
	// If there are pending scheduled units, compile the next one
	if ( currentCompilerCall < compilerCalls.size() )
	{
		// When this compilation unit has finished, call this method to compile the next one
		CompilerCall* compilerCall = compilerCalls[currentCompilerCall];
		connect(compilerCall, SIGNAL(finished()), this, SLOT(compilerCallFinished()) );

		// Start the compilation process
		compilerCall->compile();
	}
	else if ( shouldLinkExecutable )
		linkExecutable();
	else // Compilation process finished
		emit finished();
}

void Compiler::compilerCallFinished()
{
	// Get the compilation unit that finished
	CompilerCall* compilerCall = compilerCalls[currentCompilerCall];

	// Update the record of warnings and errors
	errorCount += compilerCall->getErrorCount();
	warningCount += compilerCall->getWarningCount();

	// If there are errors, an object file was not generated therefore executable cannot be linked
	if ( errorCount > 0 )
		shouldLinkExecutable = false;

	// Continue compiling the next compilation unit
	++currentCompilerCall;
	compileNextSourceFile();
}

#include <QDebug>

void Compiler::linkExecutable()
{
	// Ensambles a command, something such as
	// c++ -Wall -std=c++11 /path/player/unit/main.o /path/player/unit/MyClass.o -o /path/player/unit/unit
	QStringList arguments( CompilerCall::getDefaultCompilerArguments() );
	arguments << CompilerCall::getDefaultLinkerArguments();
	arguments << objectFiles;
	arguments << "-o" << executablePath.absoluteFilePath();

	// Call the linker
	// CALL COMPILER
	qDebug() << CompilerCall::getCxxCompiler() << arguments;
	emit finished(); // for the moment
}
