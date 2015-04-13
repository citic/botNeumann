#ifndef COMPILATIONUNIT_H
#define COMPILATIONUNIT_H

#include <QFileInfo>

class Diagnostic;
class QProcess;

/// The different states of the compilation process
enum class CompilationState
{
	unknown,
	scheduled,
	started,
	finished
};

/**
	@brief A Compilation Unit is a source file (.c, .cpp) that must be compiled as a single unit.

	When player press the Run button, all the files that comprise the solution are sent to be
	compiled. The class Compiler schedules only the files that have been changed since last
	compilation process. Each scheduled source file generates a CompiationUnit instance.

	The actual compiler is called using a QProcess system call. The diagnostics generated by the
	compiler are stored in this class
 */
class CompilationUnit : public QObject
{
	Q_OBJECT

  protected:
	/// Complete file path to the source file (.c/.cpp)
	QFileInfo sourcePath;
	/// Complete file path to the target object file (.o)
	QFileInfo targetPath;
	/// Status of the process that compiles this unit
	CompilationState state;
	/// List of warnings and errors generated when compiling this source file
	QList<Diagnostic*> diagnostics;
	/// The compiler executable is called in a separate process
	QProcess* process;

  public:
	/// Constructor
	explicit CompilationUnit(const QFileInfo& sourcePath, QObject* parent = nullptr);
	/// Destructor
	~CompilationUnit();
	/// Get access to the source file path
	inline const QFileInfo& getSourcePath() const { return sourcePath; }
	/// Get access to the target object file path
	inline const QFileInfo& getTargetPath() const { return targetPath; }
	/// Given a source file, returns its .o object file
	static QFileInfo getObjectFileFor(const QFileInfo& sourceFilePath);
	/// Returns the state of the compilation process of this unit
	inline CompilationState getState() const { return state; }
	/// Get access to the list of diagnostics (warnings and errors)
	inline const QList<Diagnostic*>& getDiagnostics() const { return diagnostics; }
	/// Erases all dignostics
	void clear();
	/// Returns true if the source file is newer than the given one
	inline bool isSourceNewerThan(const QFileInfo& fileInfo) const { return isFileNewerThan(sourcePath, fileInfo); }
	/// Returns true if the object file is newer than the given one
	inline bool isTargetNewerThan(const QFileInfo& fileInfo) const { return isFileNewerThan(targetPath, fileInfo); }
	/// Returns true if @a file1 is newer (last modified) than file2
	static bool isFileNewerThan(const QFileInfo& file1, const QFileInfo& file2);
	/// Launch a compiler process to compile this unit
	/// Emits finished() when it has finished. The result (success, error, or warnings) can be
	/// consulted after the finished() signals has been emitted
	void compile();

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

  protected slots:
	/// Called when the compiling process of this unit finished
	void processFinished();

  protected:
	/// Determines if this source file must be or not compiled. It depends if the dates of the
	/// source file, or an existing target file are already udpated. This method is called from
	/// the constructor and updates the state data member
	void updateState();
};

#endif // COMPILATIONUNIT_H