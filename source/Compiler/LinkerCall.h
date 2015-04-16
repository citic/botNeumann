#ifndef LINKERCALL_H
#define LINKERCALL_H

#include "ToolCall.h"
#include <QStringList>

class Diagnostic;

/**
	@brief Calls the linker with a buch of object files to generate an executable
 */
class LinkerCall : public ToolCall
{
	Q_OBJECT

  protected:
	/// The list of all object files (.o) to be included in the executable file
	QStringList objectFiles;
	/// The executable absolute file path to be generated or overwritten
	QFileInfo executablePath;
	/// List of warnings and errors generated when linking the executable
	QList<Diagnostic*> diagnostics;
	/// Number of errors found in this source file. If > 0, code cannot be run
	int errorCount;
	/// Number of warnings found in this source file. Solution may still run
	int warningCount;

  public:
	/// Constructor
	explicit LinkerCall(const QStringList& objectFiles, const QFileInfo& executablePath, QObject *parent = nullptr);
	/// Destructor
	~LinkerCall();
	/// Get access to the list of diagnostics (warnings and errors)
	inline const QList<Diagnostic*>& getDiagnostics() const { return diagnostics; }
	/// Number of errors found in this source file. If > 0, code cannot be run
	inline int getErrorCount() const { return errorCount; }
	/// Number of warnings found in this source file. Solution may still run
	inline int getWarningCount() const { return warningCount; }
	/// Launch a linker process to generate the executable file
	/// Emits finished() when it has finished. The result (success, error, or warnings) can be
	/// consulted after the finished() signal has been emitted
	void start();

  protected slots:
	/// Called when the compiling process of this unit finished
	void processFinished();
};

#endif // LINKERCALL_H
