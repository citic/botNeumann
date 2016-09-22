#ifndef DEBUGGERCALL_H
#define DEBUGGERCALL_H

#include "GdbResponse.h"
#include "ToolCall.h"

#include <QByteArray>
#include <QList>

class GdbOutput;
class GdbToken;
class QSocketNotifier;

class GdbCommand
{
  protected:
	static size_t instances;
	size_t number;
	QString text;

  public:
	explicit GdbCommand(const QString& text) : number(++instances), text(/*number + */text) { }
	inline size_t getNumber() const { return number; }
	inline const QString& getText() const { return text; }
	inline QString getCommand() const { return text + '\n'; }
};

class DebuggerCall : public ToolCall
{
	Q_OBJECT

  protected:
	/// Identifier of the pseudoterminal where user program will be run
	/// This id is known as Device File Descriptor un Unix jargon
	/// This pseudoterminal will be given to gdb to control inferior
	int inferiorPseudoterminalId = 0;
	/// A QSocketNotifier is used to monitor activity on a file descriptor, i.e.
	/// It will emmit signals when GDB produces output
	QSocketNotifier* pseudoterminalActivityMonitor = nullptr;
	/// Controls the number of gdb commands being run
	int busy = 0;
	/// The list of commands sent to GDB that are waiting for GDB response (output)
	QList<GdbCommand> pendingCommands;
	///
	QList<GdbToken*> pendingTokens;
	/// Raw output received from GDB process
	QByteArray gdbRawOutput;

  public:
	/// Constructor
	explicit DebuggerCall(QObject *parent = nullptr);
	/// Destructor
	virtual ~DebuggerCall();
	/// Stats the execution of gdb and inferior
	bool start();
	/// Gets the inferior pseudoterminal name, something like /dev/pts/nn where
	/// nn is a number that identifies the pseudoterminal
	const char* getInferiorPseudoterminalName() const;

  protected:
	/// Creates a pseudoterminal where user program (inferior) is going to run
	/// This pseudoterminal will be given to gdb to control inferior
	bool createPseudoterminal();
	/// Sets signals and slots to detect when GDB produces output
	void startMonitoringPseudoterminal();
	/// Sends a command to GDB, for example "-file-exec-and-symbols"
	/// Returns a parsed tree response
	GdbResponse sendGdbCommand(const QString& command);
	/// Reads a response from Gdb. It will wait if Gdb has still no responses
	void readFromGdb(GdbResponse& response);
	/// If there is output generated by GDB, parses it and generate a list of tokens
	GdbOutput* parseGdbOutput();
	///
	inline bool isTokenPending() { return peekToken() != nullptr; }
	///
	GdbToken* peekToken();
	///
	void readTokens();
	///
	void parseGdbOutputLine(const QString& line);

  private slots:
	/// Called each time GDB produces output
	void onGdbOutput(int fileDescriptor);
};

#endif // DEBUGGERCALL_H