#ifndef LINKERCALL_H
#define LINKERCALL_H

#include "ToolCall.h"
#include <QStringList>

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

  public:
	/// Constructor
	explicit LinkerCall(const QStringList& objectFiles, const QFileInfo& executablePath, QObject *parent = nullptr);
	/// Destructor
	~LinkerCall();
	/// Launch a linker process to generate the executable file
	/// Emits finished() when it has finished. The result (success, error, or warnings) can be
	/// consulted after the finished() signal has been emitted
	void start();

  protected slots:
	/// Called when the compiling process of this unit finished
	void processFinished();
};

#endif // LINKERCALL_H
