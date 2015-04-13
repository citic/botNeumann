#include "CompilationUnit.h"
#include "Diagnostic.h"
#include <QDateTime>
#include <QDir>

CompilationUnit::CompilationUnit(const QFileInfo& sourcePath, QObject* parent)
	: QObject(parent)
	, sourcePath(sourcePath)
	, targetPath(getObjectFileFor(sourcePath))
	, state( CompilationState::unknown )
{
	updateState();
}

CompilationUnit::~CompilationUnit()
{
}

void CompilationUnit::updateState()
{
	// If there exists the object file, and it is updated avoid to compile it again
	if ( targetPath.exists() && isFileNewerThan(targetPath, sourcePath) )
		state = CompilationState::finished; // No compilation required
	else
		state = CompilationState::scheduled; // Schedule this file for compilation
}

QFileInfo CompilationUnit::getObjectFileFor(const QFileInfo& sourceFilePath)
{
	return sourceFilePath.absolutePath() + QDir::separator() + sourceFilePath.completeBaseName() + ".o";
}

void CompilationUnit::clear()
{
	for (int i = 0; i < diagnostics.size(); ++i)
		delete diagnostics[i];

	diagnostics.clear();
}

bool CompilationUnit::isFileNewerThan(const QFileInfo& file1, const QFileInfo& file2)
{
	return file1.lastModified() > file2.lastModified();
}

#include <QDebug>

void CompilationUnit::compile()
{
	// Ensambles a command, something such as
	// c++ -Wall -std=c++11 -c /path/player/unit/main.cpp -o /path/player/unit/main.o
	QStringList arguments( getDefaultCompilerArguments() );
	arguments << "-c" << sourcePath.absoluteFilePath();

	// The object file is the same source chaging its extension to .o
	arguments << "-o" << targetPath.absoluteFilePath();

	// Compilation is required, call the compiler
	// CALL COMPILER
	qDebug() << getCxxCompiler() << arguments;

}

QString CompilationUnit::getCxxCompiler()
{
  #if defined(Q_OS_MACX)
	return "clang++";
  #else
	return "g++";
#endif
}

QStringList CompilationUnit::getDefaultCompilerArguments()
{
	QStringList arguments;
	arguments << "-g" << "-Wall" << "-Wextra" << "-std=c++11";
	return arguments;
}

QStringList CompilationUnit::getDefaultLinkerArguments()
{
	QStringList arguments;
  #if ! defined(Q_OS_WIN)
	arguments << "-lm";
  #endif
	return arguments;
}
