#include "ToolCall.h"
#include "Common.h"

#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>

ToolCall::ToolCall(QObject* parent)
	: QObject(parent)
	, state( ToolCallState::unknown )
	, process(nullptr)
{
}

ToolCall::~ToolCall()
{
}

bool ToolCall::isFileNewerThan(const QFileInfo& file1, const QFileInfo& file2)
{
	return isFileNewerThan(file1.absoluteFilePath(), file2.absoluteFilePath());
}

bool ToolCall::isFileNewerThan(const QString& file1, const QString& file2)
{
	return QFileInfo(file1).lastModified() > QFileInfo(file2).lastModified();
}

QString ToolCall::getCompilerInstallationDirectory()
{
	// Get installation directory from preferences
	QSettings settings;
	QString installationDirectory = settings.value(sk("Compiler/InstallationDirectory"), "").toString();
	if ( installationDirectory.length() > 0 )
		installationDirectory.append( QDir::separator() );
	return installationDirectory;
}

QString ToolCall::getCCompiler()
{
  #if defined(Q_OS_MACX)
	return getCompilerInstallationDirectory() + "clang";
  #else
	return getCompilerInstallationDirectory() + "gcc";
  #endif
}

QString ToolCall::getCxxCompiler()
{
  #if defined(Q_OS_MACX)
	return getCompilerInstallationDirectory() + "clang++";
  #else
	return getCompilerInstallationDirectory() + "g++";
#endif
}

ProgrammingLanguage ToolCall::mapProgrammingLanguage(const QString& filename)
{
	return mapProgrammingLanguage( QFileInfo(filename) );
}

ProgrammingLanguage ToolCall::mapProgrammingLanguage(const QFileInfo& fileInfo)
{
	const QString& extension = fileInfo.suffix();

	// C
	if ( extension == "c" ) return ProgrammingLanguage::c;

	// C++
	if ( extension.compare("cpp", Qt::CaseInsensitive ) == 0 ) return ProgrammingLanguage::cpp;
	if ( extension.compare("cxx", Qt::CaseInsensitive ) == 0 ) return ProgrammingLanguage::cpp;
	if ( extension.compare("cc",  Qt::CaseInsensitive ) == 0 ) return ProgrammingLanguage::cpp;
	if ( extension.compare("cp",  Qt::CaseInsensitive ) == 0 ) return ProgrammingLanguage::cpp;
	if ( extension == "C" ) return ProgrammingLanguage::cpp;

	// Oops
	Q_ASSERT(false);
	return ProgrammingLanguage::unknown;
}

QString ToolCall::getCompilerFor(ProgrammingLanguage programmingLanguage)
{
	switch ( programmingLanguage )
	{
		case ProgrammingLanguage::c: return getCCompiler();
		case ProgrammingLanguage::cpp: return getCxxCompiler();
		default: return "";
	}
}

QStringList ToolCall::getDefaultCompilerArguments(ProgrammingLanguage programmingLanguage, bool optimizeForDebug)
{
	// Common arguments for C and C++
	QStringList arguments;
	arguments << "-Wall" << "-Wextra";

	// If debug information is asked, generate it and optimize for GDB
	if ( optimizeForDebug )
		arguments << "-O0" << "-ggdb3";

	// Optional
	// arguments << "-fdiagnostics-parseable-fixits";

  #ifndef Q_OS_MACX
	// CLang produces a warning for this parameter
	arguments << "-pthread";
  #endif

	// Specific programming language arguments
	switch ( programmingLanguage )
	{
		case ProgrammingLanguage::c: arguments << "-std=c11"; break;
		case ProgrammingLanguage::cpp: arguments << "-std=c++11"; break;
		default: break; // return defaults
	}

	return arguments;
}

QStringList ToolCall::getDefaultLinkerArguments()
{
	QStringList arguments;
  #if ! defined(Q_OS_WIN)
	arguments << "-lm" << "-lpthread";
  #endif
	return arguments;
}

bool ToolCall::askPlayerCompilerDirectory()
{
	// A first dialog informs players about the problem and let them decide if they want to
	// choose the compiler's installation directory or cancel everything
	QString message(tr("A GCC compatible compiler is required in order to build and run your "
		"solution. If you have an installed compiler that is not included in your PATH environment "
		"variable, please press 'Choose directory' and locate its 'bin' subdirectory. "
		"If you do not have a compiler installed, consider the following suggestion:\n\n"));

  #if defined(Q_OS_WIN)
	message.append(tr("TDM-GCC"));
  #elif defined(Q_OS_MAC)
	message.append(tr("XCode and Command Line Tools\n"));
  #else //elif defined(Q_OS_UNIX)
	message.append(tr("GCC or Clang\n"));
  #endif

	// Ask the question to the user
	if ( QMessageBox::question(nullptr, tr("Compiler not found"), message, tr("Choose directory"), tr("Cancel")) == 0 )
	{
		// User decided to choose a directory, present a file chooser dialog
		QFileDialog dirDialog;
		dirDialog.setFileMode(QFileDialog::DirectoryOnly);
		dirDialog.setOption(QFileDialog::ShowDirsOnly);

		// Launch the file selector dialog
		if ( dirDialog.exec() )
		{
			// ToDo: check the directory actually has a compiler
			// ToDo: set the PATH enviroment variable to include that directory

			// Store the directory in the user configuration
			QSettings settings;
			settings.setValue(sk("Compiler/InstallationDirectory"), dirDialog.selectedFiles()[0] );
			return true;
		}
	}
	return false;
}
