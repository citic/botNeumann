#include "CompilationUnit.h"
#include "Compiler.h"
#include "Diagnostic.h"
#include <QStringList>

Compiler::Compiler(QObject *parent)
	: QObject(parent)
	, errorCount(-1)
	, currentCompilationUnit(-1)
{
}

Compiler::~Compiler()
{
	clear();
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
	scheduleCompilationUnits(filepaths);

	// Start to compile the first scheduled unit
	currentCompilationUnit = 0;
	compileNextUnit();
}

void Compiler::clear()
{
	for (int i = 0; i < compilationUnits.size(); ++i)
		delete compilationUnits[i];

	compilationUnits.clear();
	objectFiles.clear();
}

void Compiler::compileNextUnit()
{
	// If there are pending scheduled units, compile the next one
	if ( currentCompilationUnit < compilationUnits.size() )
	{
		// When this compilation unit has finished, call this method to compile the next one
		CompilationUnit* compilationUnit = compilationUnits[currentCompilationUnit];
		connect(compilationUnit, SIGNAL(finished()), this, SLOT(compileNextUnit()) );

		// Start the compilation process and move to the next one
		compilationUnit->compile();
		++currentCompilationUnit;
	}
	else if ( shouldLinkExecutable )
		linkExecutable();
}

#include <QDebug>

void Compiler::linkExecutable()
{
	// Ensambles a command, something such as
	// c++ -Wall -std=c++11 /path/player/unit/main.o /path/player/unit/MyClass.o -o /path/player/unit/unit
	QStringList arguments( CompilationUnit::getDefaultCompilerArguments() );
	arguments << CompilationUnit::getDefaultLinkerArguments();
	arguments << objectFiles;
	arguments << "-o" << executablePath.absoluteFilePath();

	// Call the linker
	// CALL COMPILER
	qDebug() << CompilationUnit::getCxxCompiler() << arguments;
}

void Compiler::scheduleCompilationUnits(const QFileInfoList& filepaths)
{
	// For each file, schedule these that require generate its object file
	foreach ( QFileInfo fileInfo, filepaths )
	{
		// Analyze if this source file must be compiled
		CompilationUnit* compilationUnit = new CompilationUnit(fileInfo);

		// Regardless this file should be compiled or not, it must be included in the linking phase
		objectFiles.append( compilationUnit->getTargetPath().absoluteFilePath() );

		// If this .cpp file, it is is newer than the executable, the executable must be updated
		if ( shouldLinkExecutable == false && compilationUnit->isTargetNewerThan(executablePath) )
			shouldLinkExecutable = true;

		// If the file must be compiled, schedule it, else, remove it
		if ( compilationUnit->getState() != CompilationState::finished )
			compilationUnits.append(compilationUnit);
		else
			delete compilationUnit;
	}
}
