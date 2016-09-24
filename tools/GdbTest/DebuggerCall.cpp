#include "DebuggerCall.h"
#include <QSocketNotifier>

/*static*/ size_t GdbCommand::instances = 0;

static const QString gdbPath = QStringLiteral(
#ifdef Q_OS_MAC
	"/opt/local/bin/ggdb");
#else
	"gdb");
#endif

DebuggerCall::DebuggerCall(QObject *parent)
	: ToolCall("DebuggerCall", parent)
{
}

DebuggerCall::~DebuggerCall()
{
	// Exit gdb cleanly
	printf("%s: exiting gdb...\n", qPrintable(toolName));
	process.write("-gdb-exit\n");
	process.terminate();
	process.waitForFinished();
}

bool DebuggerCall::start()
{
	if ( ! createPseudoterminal() ) return false;

	const QString& command = QStringLiteral("%1 -q --interpreter=mi2").arg(gdbPath);
	printf("%s: starting: %s\n", qPrintable(toolName), qPrintable(command));
	process.start(command);
	process.waitForStarted();
	if ( process.state() == QProcess::NotRunning )
		return false;

	/// Tells GDB to use the pseudoterminal in order to control de inferior
	sendGdbCommand( QStringLiteral("-inferior-tty-set %1").arg(getInferiorPseudoterminalName()) );

	// ...

	// Success
	return true;
}

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
bool DebuggerCall::createPseudoterminal()
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
		fprintf(stderr, "GdbTest: Failed to grant pseudoterminal %i\n", inferiorPseudoterminalId);
		close(inferiorPseudoterminalId);
		return false;
	}

	// New pseudoterminals are locked by default, in order perform whatever initialization before
	// programs start running on it. When initialization is done, pseudoterminal must be unlocked
	if( unlockpt(inferiorPseudoterminalId) != EXIT_SUCCESS )
	{
		fprintf(stderr, "GdbTest: Failed to unlock pseudoterminal %i\n", inferiorPseudoterminalId);
		close(inferiorPseudoterminalId);
		return false;
	}

	// If the pseudoterminal was created successfully, it will have a name like /dev/pts/nn where
	// nn is a number that identifies the pseudoterminal
	printf("GdbTest: using pseudoterminal: %s\n", ptsname(inferiorPseudoterminalId));
	return true;
}

void DebuggerCall::startMonitoringPseudoterminal()
{
	// QSocketNotifier monitors activity on a file descriptor
	this->pseudoterminalActivityMonitor = new QSocketNotifier(inferiorPseudoterminalId, QSocketNotifier::Read);
	// Read any output generated by GDB
	connect(this->pseudoterminalActivityMonitor, SIGNAL(activated(int)), this, SLOT(onGdbOutput(int)));
}

void DebuggerCall::onGdbOutput(int fileDescriptor)
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
	printf("gdb:'%s'\n", buffer);
}

const char*DebuggerCall::getInferiorPseudoterminalName() const
{
	return ptsname(inferiorPseudoterminalId);
}

GdbResponse DebuggerCall::sendGdbCommand(const QString& command)
{
	Q_ASSERT(busy == 0);
	++busy;

	GdbCommand gdbCommand(command);
	pendingCommands.append(gdbCommand);

	printf("Send GDB: '%s'", qPrintable(gdbCommand.getCommand()));
	process.write( qPrintable( gdbCommand.getCommand() ) );

	// Discard output from previous commands, if any
	// Read the output that GDB generates by the new sent command
	GdbResponse response;
	do
	{
		// readFromGdb matches the GDB output to each command, and
		// removes the answered commands from the pending list
		readFromGdb(response);
	} while ( pendingCommands.isEmpty() == false );

//	while( ! m_list.isEmpty() )
//		readFromGdb(NULL,resultData);

	--busy;

//	dispatchResp();
//	onReadyReadStandardOutput();

	return response;
}

void DebuggerCall::readFromGdb(GdbResponse& /*response*/)
{
	// ToDo: see com.cpp:851-878 when m_result==NULL

	GdbOutput* gdbOutput = nullptr;

	do
	{
		// Wait until GDB produces output and parse it when it becomes available
		while ( (gdbOutput = parseGdbOutput()) == nullptr )
			process.waitForReadyRead(100);

		while( processedTokens.isEmpty() == false )
			delete processedTokens.takeFirst();

//		m_respQueue.push_back(gdbOutput);

//		if(gdbOutput->getType() == GdbOutput::RESULT)
//		{
//			response.setResult( gdbOutput->getResult() );
//			m_resultData->copy(gdbOutput->tree);
//		}

	} while ( gdbOutput->getType() != GdbOutput::TERMINATION );

/*
	// Dump all stderr content
	QByteArray stderrBuffer = m_process.readAllStandardError();
	if(!stderrBuffer.isEmpty())
	{
		QString respString = QString(stderrBuffer);
		QStringList respList = respString.split("\n");
		for(int r = 0;r < respList.size();r++)
		{
			QString row = respList[r];
			if(!row.isEmpty())
				debugMsg("GDB|E>%s", stringToCStr(row));
		}
	}
*/
}

GdbOutput* DebuggerCall::parseGdbOutput()
{
	// If there not tokens to parse, stop
	if( ! isTokenPending() )
		return nullptr;

	GdbOutput* resp = nullptr;

	// [1] try to parse out of band records:
	// [1.1] parse async records:
	if ( ( resp = parseAsyncRecord(GdbToken::KEY_STAR, GdbOutput::EXEC_ASYNC_OUTPUT) ) )
		return resp;
	if ( ( resp = parseAsyncRecord(GdbToken::KEY_PLUS, GdbOutput::STATUS_ASYNC_OUTPUT) ) )
		return resp;
	if ( ( resp = parseAsyncRecord(GdbToken::KEY_EQUAL, GdbOutput::NOTIFY_ASYNC_OUTPUT) ) )
		return resp;

	// [1.2] stream records:
//		if ( isTokenPending() && resp == nullptr )
//			resp = parseStreamRecord();

// [2] result record
//	if ( isTokenPending() && resp == nullptr )
//		resp = parseResultRecord();

	// [3] termination
	resp = new GdbOutput(GdbOutput::UNKNOWN);
	GdbToken *token = checkAndPopToken(GdbToken::END_CODE);
	if( token ) resp->setType(GdbOutput::TERMINATION);

	return resp;
}

GdbToken* DebuggerCall::peekToken()
{
	readTokens();

	if ( pendingTokens.empty() )
		return nullptr;

	return pendingTokens.first();
}

GdbToken* DebuggerCall::popToken()
{
	if( pendingTokens.empty() )
		return nullptr;

	GdbToken* token = pendingTokens.takeFirst();
	processedTokens.append( token );
	fprintf(stderr, "popToken: %s\n", qPrintable(token->getText()));
	return token;
}

GdbToken* DebuggerCall::checkAndPopToken(GdbToken::Type tokenType)
{
	GdbToken* token = peekToken();

	if ( token == nullptr )
		return nullptr;

	if ( token->getType() == tokenType )
		popToken();

	return token;
}

GdbToken* DebuggerCall::eatToken(GdbToken::Type tokenType)
{
	GdbToken* token = nullptr;

	while( ( token = peekToken() ) == nullptr )
	{
		process.waitForReadyRead(100);
		readTokens();
	}

	if ( token->getType() == tokenType )
		return popToken();

	fprintf(stderr, "Expected '%s' but got '%s'\n"
		, GdbToken::typeToString(tokenType)
		, token ? qPrintable(token->getText()) : "<null>");

	return nullptr;
}

void DebuggerCall::readTokens()
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

void DebuggerCall::parseGdbOutputLine(const QString& line)
{
	if( line.isEmpty() )
		return;

	fprintf(stderr, "GDB line: [%s]\n", qPrintable(line));

	char firstChar = line[0].toLatin1();
	if( strchr("(^*+~@&=", firstChar) )
		pendingTokens.append( GdbToken::tokenize(line) );
//	else if(m_listener)
//		m_listener->onTargetStreamOutput(line);
}

GdbOutput* DebuggerCall::parseAsyncRecord(GdbToken::Type tokenType, GdbOutput::Type outputType)
{
	// If there is a variable token, discard it. Why?
	checkAndPopToken(GdbToken::VAR);

	// If there is a token of the given type, pop it
	if ( checkAndPopToken(tokenType) )
	{
		// ToDo: Who deletes this object?
		GdbOutput* resp = new GdbOutput(outputType);

		// The type of async-message comes within a VAR token
		GdbToken* tokenVar = eatToken(GdbToken::VAR);
		if ( tokenVar )
		{
			resp->parseAsyncOutput( tokenVar->getText() );
//			while ( checkAndPopToken(GdbToken::KEY_COMMA) )
//				parseResult( resp->tree.getRoot() );
		}
		return resp;
	}

	return nullptr;
}

