#include "GdbCall.h"
#include "GdbItemTree.h"
#include "LogManager.h"

#include <QProcess>
#include <QSocketNotifier>

/*static*/ size_t GdbCommand::instances = 0;

static const QString gdbPath = QStringLiteral("gdb");

GdbCall::GdbCall(QObject *parent)
	: DebuggerCall(parent)
{
}

GdbCall::~GdbCall()
{
	// Exit gdb cleanly
	qCWarning(logDebugger, "GdbCall: exiting gdb...");
	if (process)
	{
		process->write("-gdb-exit\n");
		process->terminate();
		process->waitForFinished();
	}

	// Delete dynamic allocated objects
	deleteProcessedTokens();
	while ( ! responseQueue.isEmpty() )
		delete responseQueue.takeFirst();

	// Delete any served response, if any
	delete lastServedResponse;
}

bool GdbCall::start()
{
	// Call the debugger in another process
	Q_ASSERT(process == nullptr);
	process = new QProcess(this);

	// When GDB has generated output, parse it
	connect( process, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()) );

	// We create a fake command representing the gdb command call itself, because just calling
	// gdb produces responses, and we want to match them
	const QString& command = QStringLiteral("%1 -q -i mi").arg(gdbPath);
	pendingCommands.append( GdbCommand('$'+ command, 0) );
	qCInfo(logDebuggerRequest).noquote() << command;

	// ToDo: remove synchronous waitForStarted, but the delay may be useful for other tool calls?
	process->start(command);
	process->waitForStarted();
	if ( process->state() == QProcess::NotRunning )
		return false;

	// Success
	return true;
}

bool GdbCall::stop()
{
	bool result = sendGdbCommand("-exec-interrupt", 0) != GDB_ERROR;
	busy = -100;
	return result;
}

GdbResult GdbCall::sendGdbCommand(const QString& command, int userData, GdbItemTree* resultData)
{
	// If GDB is already running another command, wait
	if ( busy != 0 )
		return GDB_ERROR;

	// Now we are sending another command
	++busy;

	// Create an object representing the command and add it to the list of commands being processed
	GdbCommand gdbCommand(command, userData);
	pendingCommands.append(gdbCommand);

	// Ugly fix: GDB does not reply command numbers with "-exec-run" command, we enforce it
	if ( command.startsWith("-exec-run") )
	{
		lastCommandNumberReceived = gdbCommand.getNumber();
		lastUserData = gdbCommand.getUserData();
		qCDebug(logTemporary).nospace() << "Enforcing cmd=" << lastCommandNumberReceived << " usr=" << lastUserData;
	}

	qCInfo(logDebuggerRequest).noquote().nospace() << gdbCommand.getText() << " | usr=" << userData;
	process->write( qPrintable( gdbCommand.getCommand() ) );

	GdbResult lastResult = GDB_UNKNOWN;

	// Discard output from previous commands, if any
	// Read the output that GDB generates by the new sent command
	do
	{
		// readFromGdb matches the GDB output to each command, and
		// removes the answered commands from the pending list
		lastResult = readFromGdb(resultData);
	} while ( pendingCommands.count() > 1 );

	// If GDB is generating more output, but we have not received it entirely
	while( ! pendingTokens.isEmpty() )
		readFromGdb( nullptr, false );

	--busy;

	// If busy is negative, visualization is stopped
	if ( busy >= 0 )
	{
		emit pendingGdbResponses();
		onReadyReadStandardOutput();
	}

	return lastResult;
}


void GdbCall::onReadyReadStandardOutput()
{
	// If already parsing GDB output, stop
	if ( busy ) return;

	while ( process->bytesAvailable() || pendingTokens.isEmpty() == false )
	{
		readFromGdb(nullptr, false);
//		Q_ASSERT( pendingCommands.isEmpty() == true );
	}

	// If busy is negative, visualization is stopped
	if ( busy >= 0 )
		emit pendingGdbResponses();
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
				process->waitForReadyRead(100);
		}
		while ( response == nullptr && waitUntilGdbHasOutput );

		if ( response )
		{

			deleteProcessedTokens();

			// If we start to receive answer for a new command
			if ( response->getCommandNumber() > this->lastCommandNumberReceived )
			{
				lastCommandNumberReceived = response->getCommandNumber();

				// Try to find the pending command that generated the new train of responses
				GdbCommand* command = findPendingCommandWithNumber(lastCommandNumberReceived);
				if ( command == nullptr )
					qCDebug(logApplication) << "Response for command not found" << lastCommandNumberReceived;
				lastUserData = command ? command->getUserData() : 0;
			}
			else if ( response->getCommandNumber() == 0 )
			{
				response->setCommandNumber( lastCommandNumberReceived );
			}
			response->setUserData( lastUserData );

			//qCDebug(logTemporary) << "readFromGdb2:" << response->buildDescription(true);
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
				{
					*resultData = response->getItemTree();
					// commandNumber-1; Async output is generated by GDB before the NN^done result
					resultData->getRoot()->setTextValue( concatenateTextResponsesForCommand( response->getCommandNumber() - 1 ) );
				}
			}

			// GDB stops generating responses when it shows "(gdb)" prompt, called termination
		}
	} while ( waitUntilGdbHasOutput && response->getType() != GdbResponse::TERMINATION );

	dumpGdbStandardError();

	return result;
}

GdbCommand* GdbCall::findPendingCommandWithNumber(size_t number)
{
	for ( int index = 0; index < pendingCommands.count(); ++index )
		if ( pendingCommands[index].getNumber() == number )
			return & pendingCommands[index];

	return nullptr;
}

QString GdbCall::concatenateTextResponsesForCommand(size_t commandNumber)
{
	QString result;
	for ( int index = 0; index < responseQueue.count(); ++index )
		if ( responseQueue[index]->getCommandNumber() == commandNumber )
			result += responseQueue[index]->getText();

	return result;
}

void GdbCall::dumpGdbStandardError()
{
	// Dump all stderr content
	QByteArray stderrBuffer = process->readAllStandardError();

	// Continue only if there are standard error messages
	if ( stderrBuffer.isEmpty() )
		return;

	// Slice messages into lines
	const QStringList& lineList = QString(stderrBuffer).split("\n");

	// Print each line using custom format
	for ( int lineIndex = 0; lineIndex < lineList.size(); ++lineIndex )
		if ( lineList[lineIndex].isEmpty() == false )
			qCCritical(logDebugger) << "GDB error:" << lineList[lineIndex];
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
	resp = new GdbResponse(GdbResponse::UNKNOWN, lastUserData);
	GdbToken* token = checkAndPopToken(GdbToken::END_CODE);
	if ( token )
	{
		resp->setType(GdbResponse::TERMINATION);
		resp->setCommandNumber( token->getCommandNumber() );
	}

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

  #ifdef LOG_GDB_PARSER
	qCDebug(logDebugger) << "popToken:" << token->buildDescription();
  #endif
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
		process->waitForReadyRead(100);
		readTokens();
	}

	if ( token->getType() == tokenType )
		return popToken();

	qCDebug(logDebugger).noquote() << QString("GdbCall::eatToken: Expected '%1' but got '%1'")
		.arg( GdbToken::mapTypeToString(tokenType) )
		.arg( token ? qPrintable(token->getText()) : "<null>" );

	return nullptr;
}

void GdbCall::deleteProcessedTokens()
{
	while ( processedTokens.isEmpty() == false )
		delete processedTokens.takeFirst();
}

void GdbCall::readTokens()
{
	gdbRawOutput += process->readAllStandardOutput();

	// Any characters received?
	while ( gdbRawOutput.size() > 0 )
	{
		// Newline received?
		int newLineIndex = gdbRawOutput.indexOf('\n');
		if ( newLineIndex != -1 )
		{
			QString line = QString( gdbRawOutput.left(newLineIndex) );
			gdbRawOutput = gdbRawOutput.mid( newLineIndex + 1 );
			parseGdbOutputLine(line);
		}
		else if ( gdbRawOutput.isEmpty() == false )
		{
			// Half a line received
			int timeout = 20;
			// Wait for the complete line to be received
			while ( gdbRawOutput.indexOf('\n') == -1 )
			{
				process->waitForReadyRead(100);
				gdbRawOutput += process->readAllStandardOutput();
				--timeout;
				Q_ASSERT(timeout > 0);
			}
		}
	}
}

void GdbCall::parseGdbOutputLine(const QString& line)
{
	// We received from GDB a row output line
	if ( line.isEmpty() )
		return;

	qCInfo(logDebuggerResponse).noquote() << line;

	// The first characters may be the command number, count digits
	int lastNumberIndex = 0;
	while ( line[lastNumberIndex].isDigit() )
		++lastNumberIndex;

	// Extract the command number from first chars
	size_t commandNumber = 0;
	if ( lastNumberIndex > 0 )
		commandNumber = line.mid(0, lastNumberIndex).toULongLong();

	// The remaining of the line contains the response to be parsed
	const QString& line2 = lastNumberIndex > 0 ? line.mid(lastNumberIndex) : line;

	// Lines must start only with some allowed characters according to GDB
	char firstChar = line2[0].toLatin1();
	if ( strchr("(^*+~@&=", firstChar) )
	{
		// Parse the line and slice it in several tokens, we receive these tokens in a list
		//pendingTokens.append( GdbToken::tokenize(line) );
		const QList<GdbToken*>& newTokens = GdbToken::tokenize(line2);
		pendingTokens.append( newTokens );

		// If this line has a command number, add it to each token
		if ( commandNumber > 0 )
			for ( int index = 0; index < newTokens.count(); ++index )
				newTokens[index]->setCommandNumber(commandNumber);

	  #ifdef LOG_GDB_PARSER
		foreach ( GdbToken* token, newTokens )
			qCDebug(logDebugger).noquote() << "  " << token->buildDescription();
	  #endif
	}
//	else if(m_listener)
//		m_listener->onTargetStreamOutput(line);
}

GdbResponse* GdbCall::parseAsyncRecord(GdbToken::Type tokenType, GdbResponse::Type outputType)
{
	// If there is a variable token, discard it. Why?
	checkAndPopToken(GdbToken::VAR);

	// If there is a token of the given type, pop it
	GdbToken* token = nullptr;
	if ( ( token = checkAndPopToken(tokenType) ) )
	{
		// A response is a collection of tokens that answers a command
		GdbResponse* resp = new GdbResponse(outputType, lastUserData, token->getCommandNumber());

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

	GdbResponse* resp = new GdbResponse(type, lastUserData);
	GdbToken* token = eatToken(GdbToken::C_STRING);
	Q_ASSERT(token);
	resp->setText( token->getText() );
	resp->setCommandNumber( token->getCommandNumber() );
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
		qCCritical(logDebugger) << "Invalid result class found:" << resultClass;
		return nullptr;
	}

	// Create a response representing this GDB response
	GdbResponse* response = new GdbResponse(GdbResponse::RESULT, lastUserData, token->getCommandNumber());
	response->setResult(resultType);

	// If there are pairs item=values, fill the item tree
	while ( checkAndPopToken(GdbToken::KEY_COMMA) )
		if ( parseItem( response->getRootItem() ) != 0 )
			break;

	// This result token finishes a command that we issued previously to GDB
	if ( ! pendingCommands.isEmpty() )
	{
		GdbCommand cmd = pendingCommands.takeFirst();
	  #ifdef LOG_GDB_RESPONSES
		qCDebug(logDebugger).noquote() << "DONE" << cmd.getText();
	  #endif
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
		qCCritical(logDebugger) << "Unexpected token:" << token->getText();

	return result;
}

GdbResponse* GdbCall::takeNextResponse()
{
	// Delete the previous served response, if any
	delete lastServedResponse;

	// Get the next pending response, if any
	if ( responseQueue.isEmpty() )
		lastServedResponse = nullptr;
	else
		lastServedResponse = responseQueue.takeFirst();

	// Return a pointer to it
	return lastServedResponse;
}
