#ifndef COMPILERCALL_H
#define COMPILERCALL_H

#include "ToolCall.h"

class Diagnostic;

/**
	@brief A Compilation Unit is a source file (.c, .cpp) that must be compiled as a single unit.

	When player press the Run button, all the files that comprise the solution are sent to be
	compiled. The class Compiler schedules only the files that have been changed since last
	compilation process. Each scheduled source file generates a CompiationUnit instance.

	The actual compiler is called using a QProcess system call. The diagnostics generated by the
	compiler are stored in this class
 */
class CompilerCall : public ToolCall
{
	Q_OBJECT

  protected:
	/// Complete file path to the source file (.c/.cpp)
	QFileInfo sourcePath;
	/// Complete file path to the target object file (.o)
	QFileInfo targetPath;
	/// List of warnings and errors generated when compiling this source file
	QList<Diagnostic*> diagnostics;
	/// Number of errors found in this source file. If > 0, code cannot be run
	int errorCount;
	/// Number of warnings found in this source file. Solution may still run
	int warningCount;

  public:
	/// Constructor
	explicit CompilerCall(const QFileInfo& sourcePath, QObject* parent = nullptr);
	/// Destructor
	~CompilerCall();
	/// Get access to the source file path
	inline const QFileInfo& getSourcePath() const { return sourcePath; }
	/// Get access to the target object file path
	inline const QFileInfo& getTargetPath() const { return targetPath; }
	/// Given a source file, returns its .o object file
	static QFileInfo getTargetPathFor(const QFileInfo& sourceFilePath);
	/// Get access to the list of diagnostics (warnings and errors)
	inline const QList<Diagnostic*>& getDiagnostics() const { return diagnostics; }
	/// Erases all dignostics
	void clear();
	/// Number of errors found in this source file. If > 0, code cannot be run
	inline int getErrorCount() const { return errorCount; }
	/// Number of warnings found in this source file. Solution may still run
	inline int getWarningCount() const { return warningCount; }
	/// Returns true if the source file is newer than the given one
	inline bool isSourceNewerThan(const QFileInfo& fileInfo) const { return isFileNewerThan(sourcePath, fileInfo); }
	/// Returns true if the object file is newer than the given one
	inline bool isTargetNewerThan(const QFileInfo& fileInfo) const { return isFileNewerThan(targetPath, fileInfo); }
	/// Launch a compiler process to compile this unit
	/// Emits finished() when it has finished. The result (success, error, or warnings) can be
	/// consulted after the finished() signal has been emitted
	void start();

  protected slots:
	/// Called when the compiling process of this unit finished
	void processFinished();

  protected:
	/// Determines if this source file must be or not compiled. It depends if the dates of the
	/// source file, or an existing target file are already udpated. This method is called from
	/// the constructor and updates the state data member
	void updateState();
};

#endif // COMPILERCALL_H
