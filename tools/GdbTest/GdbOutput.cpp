#include "GdbOutput.h"

void GdbOutput::setType(GdbOutput::Type type)
{
	Q_ASSERT(this->type == UNKNOWN);
	this->type = type;
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

AsyncClass GdbOutput::mapAsyncText(const QString& reason)
{
	size_t countAsyncTextMap = sizeof(asyncTextMap) / sizeof(asyncTextMap[0]);
	for ( size_t index = 0; index < countAsyncTextMap; ++index )
		if ( reason == asyncTextMap[index].text )
			return asyncTextMap[index].type;

	return AsyncClass::AC_UNKNOWN;
}

int GdbOutput::parseAsyncOutput(const QString& reasonText)
{
	AsyncClass asyncType = mapAsyncText(reasonText);
	if ( asyncType == AsyncClass::AC_UNKNOWN )
	{
		fprintf(stderr, "Unexpected response '%s'\n", qPrintable(reasonText));
		Q_ASSERT(false);
		return -1;
	}

	this->reason = asyncType;
	return 0;
}
