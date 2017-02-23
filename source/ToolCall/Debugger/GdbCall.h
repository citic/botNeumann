#ifndef GDBCALL_H
#define GDBCALL_H

#include "GdbCommon.h"
#include "GdbResponse.h"
#include "GdbToken.h"
#include "DebuggerCall.h"

#include <QByteArray>
#include <QList>

class QSocketNotifier;
class GdbTreeNode;
class GdbItemTree;

class GdbCommand
{
  protected:
	static size_t instances;
	size_t number;
	QString text;
	int userData = 0;

  public:
	explicit GdbCommand(const QString& text, int userData) : number(instances++), text(text), userData(userData) { }
	inline size_t getNumber() const { return number; }
	inline QString getText() const { return QString("%1%2").arg(number).arg(text); }
	inline QString getCommand() const { return QString("%1%2\n").arg(number).arg(text); }
	inline int getUserData() const { return userData; }
};

class GdbCall : public DebuggerCall
{
	Q_OBJECT
	Q_DISABLE_COPY(GdbCall)

  protected:
	/// Controls the number of gdb commands being run
	int busy = 0;
	/// The list of commands sent to GDB that are waiting for GDB response (output)
	QList<GdbCommand> pendingCommands;
	/// Raw output received from GDB process
	QByteArray gdbRawOutput;
	/// A list of keywords, variables, texts... that result of parsing GDB output
	QList<GdbToken*> pendingTokens;
	/// A list of tokens that were previously in pendingTokens and were processed
	QList<GdbToken*> processedTokens;
	/// A list of GDB responses that were assambled from the parsed tokens
	QList<GdbResponse*> responseQueue;
	/// Keeps alive a pointer to the last response served. It will be deleted when
	/// @a takeNextResponse() is called again to serve the next response
	GdbResponse* lastServedResponse = nullptr;
	/// When we receive responses from GDB, some of them have its command number. GDB does
	/// not echoes the number in all responses. This is the last command number that we have
	/// received to the moment. This numnber is used to map the responses with the command
	/// that produced them.
	size_t lastCommandNumberReceived = 0;
	/// We preserve a copy of the last command user data, because a command may generate
	/// many responses even when the command has finished. For example command `-exec-run`
	/// finishes quickly, but it generates a cascade of responses. We preserve the userData
	/// from `-exec-run` in order to set that userData to the subsequent responses until we
	/// receive from GDB responses with a different command number.
	int lastUserData = 0;

  public:
	/// Constructor
	explicit GdbCall(QObject *parent = nullptr);
	/// Destructor
	virtual ~GdbCall();
	/// Stats the execution of gdb and inferior
	bool start();
	/// Sends a command to GDB, for example "-file-exec-and-symbols"
	/// @param command The GDB Machine Interface command
	/// @param resultData A Pointer to an existing tree where items generated by GDB
	/// will be stored, for example, to get the list of function calls on stack
	/// @return The enumeration value of the last GdbResult record: done, running, error...
	GdbResult sendGdbCommand(const QString& command, int userData = 0, GdbItemTree* resultData = nullptr);
	/// Returns a pointer to the next response that is pending to be processed
	/// @return A pointer to a response object, it will not deleted until takeNextReponse() is
	/// called again. If there are not available responses, returns null
	GdbResponse* takeNextResponse();

  protected:
	/// Reads a response from Gdb. It will wait if Gdb has still no responses
	GdbResult readFromGdb(GdbItemTree* resultData, bool waitUntilGdbHasOutput = true);
	/// Finds the pending command that has the given number
	/// @return A pointer to the command if found, nullptr otherwise
	GdbCommand* findPendingCommandWithNumber(size_t number);
	/// If there is output generated by GDB, parses it and generate a list of tokens
	GdbResponse* parseGdbOutput();
	/// Process any standard error message generated by GDB
	void dumpGdbStandardError();
	/// Returns true if there is a token ready to be processed, false otherwise
	inline bool isTokenPending() { return peekToken() != nullptr; }
	/// Deletes all processed token objects from memory, if any
	void deleteProcessedTokens();
	/// Gets a pointer to the next token that is ready to be processed, nullptr if none
	GdbToken* peekToken();
	/// Moves the first pending token (in pendingTokens list) to processedTokens list
	/// @returns A pointer to the token moved, nullptr if there is no pending tokens
	GdbToken* popToken();
	/// @brief Checks and pops a token if the kind is as expected.
	/// @return The found token or nullptr if no hit.
	GdbToken* checkAndPopToken(GdbToken::Type tokenType);
	/// Waits until there is a token
	GdbToken* eatToken(GdbToken::Type tokenType);
	/// Reads any standard output generated by GDB, slices it into lines, and parsers
	/// each line to generate tokens (lexical parsing)
	void readTokens();
	/// Parsers a line generated by GDB in standard output, and generate several tokens
	/// that will be stored into @a pendingTokens list
	void parseGdbOutputLine(const QString& line);
	/// Lines such as '=thread-group-added,id="i1"'
	GdbResponse* parseAsyncRecord(GdbToken::Type tokenType, GdbResponse::Type outputType);
	/// Lines such as '~"Any long text description"'
	GdbResponse* parseStreamRecord();
	/// Lines such as ''
	GdbResponse* parseResultRecord();
	/// Parses items in the form item=value,item=[value1,value2],item={item1,item2}
	/// @return 0 on success, -1 on error
	int parseItem(GdbTreeNode* parent);
	/// Parses the value of an item
	int parseValue(GdbTreeNode* item);

  signals:
	/// Emitted when there are GdbResponses pending to be processed
	/// Observers can fetch the next pending response with @a takeNextResponse()
	void pendingGdbResponses();

  public slots:
	/// Called when GDB process has generated output
	void onReadyReadStandardOutput();
};

#endif // GDBCALL_H
