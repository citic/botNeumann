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

/**
	@brief Base class to some build and debug tools: compiler, linker, debugger
 */
class ToolCall : public QObject
{
	Q_OBJECT

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
	static bool isFileNewerThan(const QFileInfo& file1, const QFileInfo& file2);

  public:
	/// Get the name of the default C++ compiler for this platform
	static QString getCxxCompiler();
	/// Default parameters: all warnings enabled (-Wall), C++11 enabled (-std=c++11),
	/// debug information enabled (-g), and so on
	static QStringList getDefaultCompilerArguments();
	/// Default linker arguments
	static QStringList getDefaultLinkerArguments();

  signals:
	/// Emitted when the compiling process has finished for whatever reason
	void finished();
};

#endif // TOOLCALL_H
