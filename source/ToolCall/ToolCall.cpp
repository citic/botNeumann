#include "ToolCall.h"
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
	QString installationDirectory = settings.value("Compiler/InstallationDirectory", "").toString();
	if ( installationDirectory.length() > 0 )
		installationDirectory.append( QDir::separator() );
	return installationDirectory;
}

QString ToolCall::getCxxCompiler()
{

  #if defined(Q_OS_MACX)
	return getCompilerInstallationDirectory() + "clang++";
  #else
	return getCompilerInstallationDirectory() + "g++";
  #endif
}

QStringList ToolCall::getDefaultCompilerArguments()
{
	QStringList arguments;
	arguments << "-g" << "-g3" << "-O0" << "-Wall" << "-Wextra" << "-std=c++11" << "-pthread";
//	if ( getCxxCompiler() == "clang++" )
//		arguments << "-fdiagnostics-parseable-fixits";
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
			settings.setValue("Compiler/InstallationDirectory", dirDialog.selectedFiles()[0] );
			return true;
		}
	}
	return false;
}
