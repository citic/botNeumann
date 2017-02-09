#ifndef TOOLCALL_H
#define TOOLCALL_H

#include <QFileInfo>
#include <QObject>

class QProcess;

/// The different states of the compilation process
enum class ToolCallState
{
	unknown,
	scheduled,
	started,
	finished
};

/// Supported programming languages
enum class ProgrammingLanguage
{
	unknown,
	c,
	cpp,
};


/**
	@brief Base class to some build and debug tools: compiler, linker, debugger
 */
class ToolCall : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ToolCall)

  protected:
	/// Status of the tool called in a separated process
	ToolCallState state;
	/// The compiler executable is called in a separate process
	QProcess* process;

  public:
	/// Constructor
	explicit ToolCall(QObject* parent = nullptr);
	/// Destructor
	~ToolCall();
	/// Returns the state of the compilation process of this unit
	inline ToolCallState getState() const { return state; }
	/// Returns true if @a file1 is newer (last modified) than file2
	/// This function must create new QFileInfo objects, in order to force QFileInfo re-read the
	/// times from secondary memory. It is necessary because user modifies files in edit process
	static bool isFileNewerThan(const QFileInfo& file1, const QFileInfo& file2);
	/// Returns true if @a file1 is newer (last modified) than file2
	static bool isFileNewerThan(const QString& file1, const QString& file2);

  public:
	/// Get the directory where the compiler suite being used is installed
	/// This directory may be asked to the user using a file dialog
	static QString getCompilerInstallationDirectory();
	/// Get the name of the default C compiler for this platform
	static QString getCCompiler();
	/// Get the name of the default C++ compiler for this platform
	static QString getCxxCompiler();
	/// Detects the programming language to compile the given file looking at its extension
	static ProgrammingLanguage mapProgrammingLanguage(const QString& filename);
	/// Detects the programming language to compile the given file looking at its extension
	static ProgrammingLanguage mapProgrammingLanguage(const QFileInfo& fileInfo);
	/// Detects the respective compiler for the given extension
	static QString getCompilerFor(ProgrammingLanguage programmingLanguage);
	/// Default parameters: all warnings enabled (-Wall), C++11 enabled (-std=c++11),
	/// debug information enabled (-g), and so on
	/// @param programmingLanguage If unknown is sent, only common parameters for C and C++ are
	/// returned (for example, to call the linker)
	static QStringList getDefaultCompilerArguments(ProgrammingLanguage programmingLanguage);
	/// Default linker arguments
	static QStringList getDefaultLinkerArguments();
	/// Asks to the player the installation directory of a GCC compatible compiler
	/// The compilation fails when the compiler executable is not found or when user has not
	/// permissions to execute it. The most common scenery is in Windows, when the compiler is
	/// not installed or not included in PATH variable. The given directory is stored in an
	/// user setting, so it will be reused in future executions of botNeumann
	/// @return true if player set the directory, false if player does not provide one
	bool askPlayerCompilerDirectory();

  signals:
	/// Emitted when the compiling process has finished for whatever reason
	void finished();
};

#endif // TOOLCALL_H
