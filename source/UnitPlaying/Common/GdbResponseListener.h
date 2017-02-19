#ifndef GDBRESPONSELISTENER_H
#define GDBRESPONSELISTENER_H

#include <QObject>

#include "GdbCommon.h"
#include "GdbResponse.h"
#include "VisualizationContext.h"

/** Interface for objects that animate responses fetched from Gdb **/
class GdbResponseListener : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(GdbResponseListener)

  public:
	/// Constructor
	explicit GdbResponseListener(QObject* parent = nullptr);
	/// Destructor
	virtual ~GdbResponseListener();

  public slots:
	/// Emitted when there is a GdbResponse to process.
	/// Actors are interested on these responses to create an animation in some cases, for example
	/// when a function is called, an animation of a stack frame rising from the floor is done.
	/// @param maxDuration A reference to a integer variable is passed to all actors. The actor
	/// with the largest duration to do the animation must assign this variable. The Visualizator
	/// will wait these amount of milliseconds (the animation is done) until fetching the next
	/// pending GdbResponse
	virtual void onGdbResponse(const GdbResponse* response, int& maxDuration);

  protected:
	/// Async records are used to notify the gdb/mi client of additional changes that have occurred.
	///	Those changes can either be a consequence of gdb/mi commands (e.g., a breakpoint modified)
	///	or a result of target activity (e.g., target stopped). The following is the list of possible
	///	async records:
	///		*running,thread-id="thread"
	///		*stopped,reason="reason",thread-id="id",stopped-threads="stopped",core="core"
	///	@see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	virtual void onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration);
	/// Notifications that begin with '+'
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	virtual void onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration);
	/// Notifications that begin with '=', for example '=thread-group-added,id="id"'
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Async-Records.html
	virtual void onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration);
	/// Additional out-of-band notifications:
	///   "^done[,results]": The synchronous operation was successful, results are provided
	///   "^running": Deprecated. Equivalent to ‘^done’
	///   "^connected": gdb has connected to a remote target.
	///   "^error,msg=c-string[,code=c-string]": The operation failed.
	///   "^exit": gdb has terminated.
	/// @see https://sourceware.org/gdb/onlinedocs/gdb/GDB_002fMI-Result-Records.html
	virtual void onResult(const GdbItemTree& tree, VisualizatorContext context, int& maxDuration);
	/// gdb internally maintains a number of output streams: the console (~), the target (@), and
	/// the log (&). The console output stream ('~') contains text that should be displayed in the
	/// CLI console window. It contains the textual responses to CLI commands.
	virtual void onConsoleStreamOutput(const QString& text, VisualizatorContext context, int& maxDuration);
	/// The target output stream (@) contains any textual output from the running target. This is
	/// only present when GDB's event loop is truly asynchronous, which is currently only the case
	/// for remote targets.
	virtual void onTargetStreamOutput(const QString& str, VisualizatorContext context, int& maxDuration);
	/// The log stream contains debugging messages being produced by gdb's internals.
	virtual void onLogStreamOutput(const QString& str, VisualizatorContext context, int& maxDuration);
};

/// Macro to update the maxDuration parameter in virtual functions if the duration of the current
/// animation is longer than the maxDuration value currently known
#define updateMaxDuration(code) \
	{ \
		int duration = (code); \
		if ( duration > maxDuration ) \
			maxDuration = duration; \
	} \

#endif // GDBRESPONSELISTENER_H
