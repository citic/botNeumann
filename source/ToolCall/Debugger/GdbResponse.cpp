#include "GdbResponse.h"
#include "LogManager.h"

#define arr_size(arr) (sizeof(arr) / sizeof(arr[0]))

static const char* const typeTextMap[] =
{
	"UNKNOWN",
	"RESULT",
	"CONSOLE_STREAM_OUTPUT",
	"TARGET_STREAM_OUTPUT",
	"LOG_STREAM_OUTPUT",
	"TERMINATION",
	"STATUS_ASYNC_OUTPUT",
	"NOTIFY_ASYNC_OUTPUT",
	"EXEC_ASYNC_OUTPUT",
};

const char*GdbResponse::mapTypeToString(GdbResponse::Type outputType)
{
	static_assert(arr_size(typeTextMap) == TYPE_COUNT, "Number of text must match enum");
	Q_ASSERT(outputType < TYPE_COUNT);
	return typeTextMap[outputType];
}

const struct { AsyncClass type; const char* text; } asyncTextMap[] =
{
	{ AsyncClass::AC_STOPPED, "stopped" },
	{ AsyncClass::AC_RUNNING, "running" },
	{ AsyncClass::AC_THREAD_CREATED, "thread-created" },
	{ AsyncClass::AC_THREAD_GROUP_ADDED, "thread-group-added" },
	{ AsyncClass::AC_THREAD_GROUP_STARTED, "thread-group-started" },
	{ AsyncClass::AC_LIBRARY_LOADED, "library-loaded" },
	{ AsyncClass::AC_BREAKPOINT_MODIFIED, "breakpoint-modified" },
	{ AsyncClass::AC_THREAD_EXITED, "thread-exited" },
	{ AsyncClass::AC_THREAD_GROUP_EXITED, "thread-group-exited" },
	{ AsyncClass::AC_LIBRARY_UNLOADED, "library-unloaded" },
	{ AsyncClass::AC_THREAD_SELECTED, "thread-selected" },
	{ AsyncClass::AC_DOWNLOAD, "download" },
	{ AsyncClass::AC_CMD_PARAM_CHANGED, "cmd-param-changed" },
};

AsyncClass GdbResponse::mapTextToReason(const QString& reason)
{
	for ( size_t index = 0; index < arr_size(asyncTextMap); ++index )
		if ( reason == asyncTextMap[index].text )
			return asyncTextMap[index].type;

	return AsyncClass::AC_UNKNOWN;
}

const char* GdbResponse::mapReasonToString(AsyncClass outputType)
{
	for ( size_t index = 0; index < arr_size(asyncTextMap); ++index )
		if ( outputType == asyncTextMap[index].type )
			return asyncTextMap[index].text;

	return nullptr;
}

GdbResult GdbResponse::mapTextToResult(const QString& resultClass)
{
	if ( resultClass == "done" )
		return GDB_DONE;
	if( resultClass == "running" )
		return GDB_RUNNING;
	if( resultClass == "connected" )
		return GDB_CONNECTED;
	if( resultClass == "error" )
		return GDB_ERROR;
	if( resultClass == "exit" )
		return GDB_EXIT;

	return GDB_UNKNOWN;
}

const char*GdbResponse::mapResultToString(GdbResult result)
{
	switch ( result )
	{
		case GDB_DONE: return "done";
		case GDB_RUNNING: return "running";
		case GDB_CONNECTED: return "connected";
		case GDB_ERROR: return "error";
		case GDB_EXIT: return "exit";

		default: return "unknown";
	}
}

void GdbResponse::setType(GdbResponse::Type type)
{
	Q_ASSERT(this->type == UNKNOWN);
	this->type = type;
}

int GdbResponse::parseAsyncOutput(const QString& reasonText)
{
	AsyncClass asyncType = mapTextToReason(reasonText);
	if ( asyncType == AsyncClass::AC_UNKNOWN )
	{
		qCritical(logDebugger, "Unexpected response '%s'", qPrintable(reasonText));
		Q_ASSERT(false);
		return -1;
	}

	this->reason = asyncType;
	return 0;
}

QString GdbResponse::buildDescription(bool includeItemTree) const
{
	QString description = QString("GdbOutput(%1, %2, '%3')")
		.arg( getTypeString() ).arg( getReasonString() ).arg( getText() );
	if ( result != GDB_UNKNOWN )
		description += " RESULT=" + getResultString();
	if ( includeItemTree )
		description += ' ' + itemTree.buildDescription();
	return description;
}
