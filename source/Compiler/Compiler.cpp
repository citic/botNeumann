#include "Compiler.h"
#include "Diagnostic.h"
#include <QDateTime>
#include <QDir>
#include <QStringList>
#include <QProcess>

Compiler::Compiler(QObject *parent)
	: QObject(parent)
	, errorCount(-1)
{
}

Compiler::~Compiler()
{
	clear();
}

QString Compiler::getCxxCompiler()
{
  #if defined(Q_OS_MACX)
	return "clang++";
  #else
	return "g++";
#endif
}

QStringList Compiler::getDefaultCompilerArguments()
{
	QStringList arguments;
	arguments << "-g" << "-Wall" << "-Wextra" << "-std=c++11";
	return arguments;
}

QStringList Compiler::getDefaultLinkerArguments()
{
	QStringList arguments;
  #if ! defined(Q_OS_WIN)
	arguments << "-lm";
  #endif
	return arguments;
}

QFileInfo Compiler::getObjectFileFor(const QFileInfo& sourceFilePath)
{
	return sourceFilePath.absolutePath() + QDir::separator() + sourceFilePath.completeBaseName() + ".o";
}

#if 0
bool Compiler::compile(const QString& filename, const QFileInfo& executablePath)
{
	// Clear old compilation contexts
	clear();

	// Compile the given file, a CLang translation unit will be generated in dynamic memory
	clangData->translationUnit = clang_parseTranslationUnit(clangData->index, filename.toStdString().c_str(), cxxArgs, cxxArgCount, nullptr, 0, CXTranslationUnit_None);

	// Get the number of warnings and errors detected by Clang and make store space for them
	unsigned diagnosticCount = clang_getNumDiagnostics(clangData->translationUnit);
	diagnostics.reserve(diagnosticCount);

	// Assume there are not errors and the code can be run
	bool canBeRun = true;

	// Iterate through all warnings and errors found by Clang and fill the diagnostics container
	for (unsigned i = 0; i < diagnosticCount; ++i)
	{
		// Get the diagnostic created in dynamic memory
		CXDiagnostic clangDiagnostic = clang_getDiagnostic(clangData->translationUnit, i);

		// Create an object Diagnostic and extract information from the Clang diagnostic
		Diagnostic* objDiagnotic = new Diagnostic(clangDiagnostic);
		diagnostics.append( objDiagnotic );

		if ( objDiagnotic->getSeverity() >= DiagnosticSeverity::error )
			canBeRun = false;

		// Release the dynamic memory required by the Clang diagnostic
		clang_disposeDiagnostic(clangDiagnostic);
	}

	return canBeRun;
}
#endif

void Compiler::compile(const QFileInfoList& filepaths, const QFileInfo& executablePath)
{
	// Copy the executable file path
	this->executablePath = executablePath;

	// The list of objectfiles to link
	QStringList objectFiles;

	// True if the executable must be linked
	bool shouldLinkExecutable = false;

	// For each file, generate its object file
	foreach ( QFileInfo fileInfo, filepaths )
	{
		// Compile this .cpp file, it is is newer than the executable, the executable must be updated
		bool newerThanExecutable = false;
		objectFiles.append( generateObjectFile(fileInfo, &newerThanExecutable).absoluteFilePath() );
		shouldLinkExecutable |= newerThanExecutable;
	}

	// Generate the executable
	if ( shouldLinkExecutable )
		linkExecutable(objectFiles);
}

void Compiler::clear()
{
	for (int i = 0; i < diagnostics.size(); ++i)
		delete diagnostics[i];

	diagnostics.clear();
}

#include <QDebug>

QFileInfo Compiler::generateObjectFile(const QFileInfo& sourceFilePath, bool* newerThanExecutable)
{
	// Assume the executable must be updated, at least we find an updated .o file
	if ( newerThanExecutable ) *newerThanExecutable = true;

	// Ensambles a command, something such as
	// c++ -Wall -std=c++11 -c /path/player/unit/main.cpp -o /path/player/unit/main.o
	QStringList arguments( getDefaultCompilerArguments() );
	arguments << "-c" << sourceFilePath.absoluteFilePath();

	// The object file is the same source chaging its extension to .o
	QFileInfo objectFilepath( getObjectFileFor(sourceFilePath) );
	arguments << "-o" << objectFilepath.absoluteFilePath();

	// If there exists the object file, it could be updated
	if ( objectFilepath.exists() )
	{
		// If the object file is updated, avoid to compile it again
		if ( objectFilepath.lastModified() > sourceFilePath.lastModified() )
		{
			// If the executable is newer than this object file, executable may be updated
			if ( newerThanExecutable && executablePath.exists() && executablePath.lastModified() > objectFilepath.lastModified() )
				*newerThanExecutable = false;

			// No compilation required
			return objectFilepath;
		}
	}

	// Compilation is required, call the compiler
	// CALL COMPILER
	qDebug() << getCxxCompiler() << arguments;

	// Done
	return objectFilepath;
}

void Compiler::linkExecutable(const QStringList& objectFiles)
{
	// Ensambles a command, something such as
	// c++ -Wall -std=c++11 /path/player/unit/main.o /path/player/unit/MyClass.o -o /path/player/unit/unit
	QStringList arguments( getDefaultCompilerArguments() );
	arguments << getDefaultLinkerArguments();
	arguments << objectFiles;
	arguments << "-o" << executablePath.absoluteFilePath();

	// Call the linker
	// CALL COMPILER
	qDebug() << getCxxCompiler() << arguments;
}
