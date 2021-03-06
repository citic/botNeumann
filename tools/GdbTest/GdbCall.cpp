#include "GdbCall.h"
#include "GdbItemTree.h"
#include <QDebug>
#include <QSocketNotifier>

/*static*/ size_t GdbCommand::instances = 0;

#ifdef Q_OS_MAC
	static const QString gdbPath = QStringLiteral("/opt/local/bin/ggdb");
#else
	static const QString gdbPath = QStringLiteral("gdb");
#endif

GdbCall::GdbCall(QObject *parent)
	: ToolCall("DebuggerCall", parent)
{
	connect( &process, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()) );
}

GdbCall::~GdbCall()
{
	// Exit gdb cleanly
	qDebug("%s: exiting gdb...", qPrintable(toolName));
	process.write("-gdb-exit\n");
	process.terminate();
	process.waitForFinished();

	// Delete dynamic allocated objects
	deleteProcessedTokens();
}

bool GdbCall::start()
{
	if ( ! createPseudoterminal() ) return false;

	const QString& command = QStringLiteral("%1 -q --interpreter=mi2").arg(gdbPath);
	qDebug("%s: starting: %s", qPrintable(toolName), qPrintable(command));
	process.start(command);
	process.waitForStarted();
	if ( process.state() == QProcess::NotRunning )
		return false;

	/// Tells GDB to use the pseudoterminal in order to control de inferior
	sendGdbCommand( QStringLiteral("-inferior-tty-set %1").arg(getInferiorPseudoterminalName()) );

	// Success
	return true;
}

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
bool GdbCall::createPseudoterminal()
{
	Q_ASSERT(inferiorPseudoterminalId == 0);

	// Open an unused pseudoterminal master device, identifed by the returned file descriptor
	// O_RDWR: open device for both reading and writing
	// O_NOCTTY: ask the pseudoterminal do not control the debugger or user program, we will do it
	inferiorPseudoterminalId = posix_openpt(O_RDWR | O_NOCTTY);

	// Tell to OS that this process is the owner of the pseudoterminal, and we need to read and
	// write permissions over the file descriptor. This step is unncessary on modern OSs, but it
	// is good practice to keep it for compatibility
	if( grantpt(inferiorPseudoterminalId) != EXIT_SUCCESS )
	{
		qFatal("GdbTest: Failed to grant pseudoterminal %i", inferiorPseudoterminalId);
		close(inferiorPseudoterminalId);
		return false;
	}

	// New pseudoterminals are locked by default, in order perform whatever initialization before
	// programs start running on it. When initialization is done, pseudoterminal must be unlocked
	if( unlockpt(inferiorPseudoterminalId) != EXIT_SUCCESS )
	{
		qFatal("GdbTest: Failed to unlock pseudoterminal %i", inferiorPseudoterminalId);
		close(inferiorPseudoterminalId);
		return false;
	}

	// If the pseudoterminal was created successfully, it will have a name like /dev/pts/nn where
	// nn is a number that identifies the pseudoterminal
	qInfo("GdbTest: using pseudoterminal: %s", ptsname(inferiorPseudoterminalId));
	return true;
}

void GdbCall::startMonitoringPseudoterminal()
{
	// QSocketNotifier monitors activity on a file descriptor
	this->pseudoterminalActivityMonitor = new QSocketNotifier(inferiorPseudoterminalId, QSocketNotifier::Read);
	// Read any output generated by GDB
	connect(this->pseudoterminalActivityMonitor, SIGNAL(activated(int)), this, SLOT(onGdbOutput(int)));
}

void GdbCall::onGdbOutput(int fileDescriptor)
{
	// Gdb generated output
	Q_UNUSED(fileDescriptor);

	// Read output generated by GDB
	char buffer[128];
	int n = read( this->inferiorPseudoterminalId, buffer, sizeof(buffer) - 1 );

	// If output was read successfully
	if(n > 0)
		buffer[n] = '\0';

	// For now
	qInfo("onGdbOutput:'%s'", buffer);
}

const char*GdbCall::getInferiorPseudoterminalName() const
{
	return ptsname(inferiorPseudoterminalId);
}

GdbResult GdbCall::sendGdbCommand(const QString& command, GdbItemTree* resultData)
{
	Q_ASSERT(busy == 0);
	++busy;

	GdbCommand gdbCommand(command);
	pendingCommands.append(gdbCommand);

	qDebug("\n>>> '%s' command sent", qPrintable(command));
	process.write( qPrintable( gdbCommand.getCommand() ) );

	GdbResult lastResult = GDB_UNKNOWN;

	// Discard output from previous commands, if any
	// Read the output that GDB generates by the new sent command
	do
	{
		// readFromGdb matches the GDB output to each command, and
		// removes the answered commands from the pending list
		lastResult = readFromGdb(resultData);
	} while ( pendingCommands.isEmpty() == false );

	// If GDB is generating more output, but we have not received it entirely
	while( ! pendingTokens.isEmpty() )
		readFromGdb( nullptr, false );

	--busy;

	dispatchResponses();
	onReadyReadStandardOutput();

	return lastResult;
}

GdbResult GdbCall::readFromGdb(GdbItemTree* resultData, bool waitUntilGdbHasOutput)
{
	GdbResponse* response = nullptr;
	GdbResult result = GDB_UNKNOWN;

	do
	{
		// Wait until GDB produces output and parse it when it becomes available
		do
		{
			if ( (response = parseGdbOutput()) == nullptr )
				process.waitForReadyRead(100);
		}
		while ( response == nullptr && waitUntilGdbHasOutput );

		if ( response )
		{

			qDebug("readFromGdb response: %s", qPrintable(response->buildDescription(true)));

			deleteProcessedTokens();

			// Each command sent to gdb will generate at least one response. We store them
			// for inform the call later about the result
			responseQueue.append(response);

			// If this response is the final result of a command
			if( response->getType() == GdbResponse::RESULT )
			{
				// This is the final result of the command
				result = response->getResult();

				// The caller wants also a copy of the item tree
				if ( resultData )
					*resultData = response->getItemTree();
			}

			// GDB stops generating responses when it shows "(gdb)" prompt, called termination
		}
	} while ( waitUntilGdbHasOutput && response->getType() != GdbResponse::TERMINATION );

	dumpGdbStandardError();

	return result;
}

void GdbCall::dumpGdbStandardError()
{
	// Dump all stderr content
	QByteArray stderrBuffer = process.readAllStandardError();

	// Continue only if there are standard error messages
	if ( stderrBuffer.isEmpty() )
		return;

	// Slice messages into lines
	const QStringList& lineList = QString(stderrBuffer).split("\n");

	// Print each line using custom format
	for ( int lineIndex = 0; lineIndex < lineList.size(); ++lineIndex )
		if ( lineList[lineIndex].isEmpty() == false )
			qDebug( "GDB error: %s", qPrintable(lineList[lineIndex]) );
}

GdbResponse* GdbCall::parseGdbOutput()
{
	// If there not tokens to parse, stop
	if( ! isTokenPending() )
		return nullptr;

	GdbResponse* resp = nullptr;

	// [1] try to parse out of band records:
	// [1.1] async records:
	if ( ( resp = parseAsyncRecord(GdbToken::KEY_STAR, GdbResponse::EXEC_ASYNC_OUTPUT) ) )
		return resp;
	if ( ( resp = parseAsyncRecord(GdbToken::KEY_PLUS, GdbResponse::STATUS_ASYNC_OUTPUT) ) )
		return resp;
	if ( ( resp = parseAsyncRecord(GdbToken::KEY_EQUAL, GdbResponse::NOTIFY_ASYNC_OUTPUT) ) )
		return resp;
	// [1.2] stream records:
	if ( ( resp = parseStreamRecord() ) )
		return resp;
	// [2] result record
	if ( ( resp = parseResultRecord() ) )
		return resp;
	// [3] termination
	resp = new GdbResponse(GdbResponse::UNKNOWN);
	if ( checkAndPopToken(GdbToken::END_CODE) )
		resp->setType(GdbResponse::TERMINATION);

	return resp;
}

GdbToken* GdbCall::peekToken()
{
	readTokens();

	if ( pendingTokens.empty() )
		return nullptr;

	return pendingTokens.first();
}

GdbToken* GdbCall::popToken()
{
	if( pendingTokens.empty() )
		return nullptr;

	GdbToken* token = pendingTokens.takeFirst();
	processedTokens.append( token );
	qDebug("popToken: %s", qPrintable(token->buildDescription()));
	return token;
}

GdbToken* GdbCall::checkAndPopToken(GdbToken::Type tokenType)
{
	GdbToken* token = peekToken();

	if ( token == nullptr )
		return nullptr;

	if ( token->getType() == tokenType )
		return popToken();

	return nullptr;
}

GdbToken* GdbCall::eatToken(GdbToken::Type tokenType)
{
	GdbToken* token = nullptr;

	while( ( token = peekToken() ) == nullptr )
	{
		process.waitForReadyRead(100);
		readTokens();
	}

	if ( token->getType() == tokenType )
		return popToken();

	qFatal("eatToken: Expected '%s' but got '%s'"
		, GdbToken::mapTypeToString(tokenType)
		, token ? qPrintable(token->getText()) : "<null>");

	return nullptr;
}

void GdbCall::deleteProcessedTokens()
{
	while( processedTokens.isEmpty() == false )
		delete processedTokens.takeFirst();
}

void GdbCall::readTokens()
{
	gdbRawOutput += process.readAllStandardOutput();

	// Any characters received?
	while( gdbRawOutput.size() > 0 )
	{
		// Newline received?
		int newLineIndex = gdbRawOutput.indexOf('\n');
		if ( newLineIndex != -1 )
		{
			QString line = QString( gdbRawOutput.left(newLineIndex) );
			gdbRawOutput = gdbRawOutput.mid( newLineIndex + 1 );
			parseGdbOutputLine(line);
		}
		else if( gdbRawOutput.isEmpty() == false )
		{
			// Half a line received
			int timeout = 20;
			// Wait for the complete line to be received
			while( gdbRawOutput.indexOf('\n') == -1 )
			{
				process.waitForReadyRead(100);
				gdbRawOutput += process.readAllStandardOutput();
				--timeout;
				Q_ASSERT(timeout > 0);
			}
		}
	}
}

void GdbCall::parseGdbOutputLine(const QString& line)
{
	if( line.isEmpty() )
		return;

	qDebug("parseGdbOutputLine: [%s]", qPrintable(line));

	char firstChar = line[0].toLatin1();
	if ( strchr("(^*+~@&=", firstChar) )
	{
		//pendingTokens.append( GdbToken::tokenize(line) );
		const QList<GdbToken*> newTokens = GdbToken::tokenize(line);
		pendingTokens.append( newTokens );
		foreach ( GdbToken* token, newTokens )
			qDebug("  %s", qPrintable(token->buildDescription()) );
	}
//	else if(m_listener)
//		m_listener->onTargetStreamOutput(line);
}

GdbResponse* GdbCall::parseAsyncRecord(GdbToken::Type tokenType, GdbResponse::Type outputType)
{
	// If there is a variable token, discard it. Why?
	checkAndPopToken(GdbToken::VAR);

	// If there is a token of the given type, pop it
	if ( checkAndPopToken(tokenType) )
	{
		// A response is a collection of tokens that answers a command
		GdbResponse* resp = new GdbResponse(outputType);

		// The type of async-message must come immediately after, within a VAR token, e.g when gdb
		// starts, it issues the assnc notification '=thread-group-added,id="i1"'
		GdbToken* tokenVar = eatToken(GdbToken::VAR);
		if ( tokenVar )
		{
			resp->parseAsyncOutput( tokenVar->getText() );
			while ( checkAndPopToken(GdbToken::KEY_COMMA) )
				parseItem( resp->getRootItem() );
		}
		return resp;
	}

	return nullptr;
}

GdbResponse* GdbCall::parseStreamRecord()
{
	GdbResponse::Type type = GdbResponse::UNKNOWN;

	if ( checkAndPopToken(GdbToken::KEY_TILDE) )
		type = GdbResponse::CONSOLE_STREAM_OUTPUT;
	else if( checkAndPopToken(GdbToken::KEY_SNABEL) )
		type = GdbResponse::TARGET_STREAM_OUTPUT;
	else if( checkAndPopToken(GdbToken::KEY_AND) )
		type = GdbResponse::LOG_STREAM_OUTPUT;

	if ( type == GdbResponse::UNKNOWN )
		return nullptr;

	GdbResponse* resp = new GdbResponse(type);
	GdbToken* token = eatToken(GdbToken::C_STRING);
	Q_ASSERT(token);
	resp->setText( token->getText() );
	return resp;
}

GdbResponse* GdbCall::parseResultRecord()
{
	// If there is a variable token, remove it, why? Original comment: Parse 'token'
	checkAndPopToken(GdbToken::VAR);

	// Result records begin with '^', for example '^done'
	if ( checkAndPopToken(GdbToken::KEY_UP) == nullptr )
		return nullptr;

	// Parse 'result class', i.e. the word after the '^', such as 'done'
	GdbToken* token = eatToken(GdbToken::VAR);
	if ( token == nullptr )
		return nullptr;

	// Map the word to the corresponding enumeration value: done, running, connected, error, exit
	QString resultClass = token->getText();
	GdbResult resultType = GdbResponse::mapTextToResult(resultClass);
	if ( resultType == GdbResult::GDB_UNKNOWN )
	{
		qCritical("Invalid result class found: %s", qPrintable(resultClass));
		return nullptr;
	}

	// Create a response representing this GDB response
	GdbResponse* response = new GdbResponse(GdbResponse::RESULT);
	response->setResult(resultType);

	// If there are pairs item=values, fill the item tree
	while ( checkAndPopToken(GdbToken::KEY_COMMA) )
		if ( parseItem( response->getRootItem() ) != 0 )
			break;

	// This result token finishes a command that we issued previously to GDB
	if ( ! pendingCommands.isEmpty() )
	{
		GdbCommand cmd = pendingCommands.takeFirst();
		qDebug("<<< '%s' command done!\n", qPrintable( cmd.getText() ));
	}

	return response;
}

int GdbCall::parseItem(GdbTreeNode* parent)
{
	// Create a node in the tree for this item
	GdbTreeNode* item = new GdbTreeNode();
	parent->addChild(item);

	// The item is compound of tokens, the next one indicates the item type
	GdbToken* token = peekToken();
	if ( token != nullptr && token->getType() == GdbToken::KEY_LEFT_BRACE )
	{
		// The item has object form: item={item1=value1,item2=value2...}, i.e. recursive parsing
		parseValue(item);
	}
	else
	{
		// The must have variable form: item=value, i.e. stop condition of recursion
		GdbToken *tokenVar = eatToken(GdbToken::VAR);
		if ( tokenVar == nullptr )
			return -1;

		// Copy the variable name as name of the item
		item->setName( tokenVar->getText() );

		// Ignore the equals sign from item=value
		if ( eatToken(GdbToken::KEY_EQUAL) == nullptr )
			return -1;

		// Parse item value
		parseValue(item);
	}

	// Success
	return 0;
}

int GdbCall::parseValue(GdbTreeNode* item)
{
	int result = 0;

	// The next token after the equals sign says the type of value:
	// item=value or item={...} or item=[...]
	GdbToken* token = popToken();

	if ( token->getType() == GdbToken::C_STRING ) // "value"
	{
		// Constant literal
		item->setTextValue( token->getText() );
	}
	else if ( token->getType() == GdbToken::KEY_LEFT_BRACE ) // {...
	{
		// Values are a tuple of comma-seaparated items:
		// item={item1=value1,item2=value2,...,itemN=valueN}
		do
		{
			// Recursiveley process each item=value within the tuple
			// the current item will be the parent for the new items
			parseItem(item);
		} while ( checkAndPopToken(GdbToken::KEY_COMMA) != nullptr );

		// Skip the closing '}' token
		if ( eatToken(GdbToken::KEY_RIGHT_BRACE) == NULL)
			return -1;
	}
	else if ( token->getType() == GdbToken::KEY_LEFT_BAR ) // [...
	{
		// Values are a list of comma-separated values:
		// item=[value1,value2,...,valueN]
		if ( checkAndPopToken(GdbToken::KEY_RIGHT_BAR) != nullptr )
		{
			// Empty lists are valid: item=[]
			return 0;
		}

		// The list is not empty, i.e. it has the form: item=[value1...]
		token = peekToken();
		if ( token->getType() == GdbToken::VAR )
		{
			// We found name of an item, i.e. it is a comma-separated list of items:
			// item=[item1=value1,item2=value2,itemN=valueN]
			do
			{
				parseItem(item);
			} while ( checkAndPopToken(GdbToken::KEY_COMMA) != nullptr );
		}
		else
		{
			// ?
			int index = 0;

			do
			{
				GdbTreeNode* node = new GdbTreeNode();
				node->setName( QString::number(++index) );
				item->addChild(node);
				result = parseValue(node);
			} while ( checkAndPopToken(GdbToken::KEY_COMMA) != nullptr );
		}

		if ( eatToken(GdbToken::KEY_RIGHT_BAR) == nullptr )
			return -1;
	}
	else
		qFatal("Unexpected token: '%s'", qPrintable(token->getText()));

	return result;
}

void GdbCall::dispatchResponses()
{
	// Dispatch each response to the listener, if any
	while ( responseQueue.isEmpty() == false )
	{
		GdbResponse* response = responseQueue.takeFirst();
		Q_ASSERT(response);

		// Dispatch the response
		emit onGdbResponse(response);

		delete response;
	}
}

void GdbCall::onReadyReadStandardOutput()
{
	// If already parsing GDB output, stop
	if ( busy ) return;

	while ( process.bytesAvailable() || pendingTokens.isEmpty() == false )
	{
		readFromGdb(nullptr, false);
		Q_ASSERT( pendingCommands.isEmpty() == true );
	}

	dispatchResponses();
}
