#include "GdbResponseListener.h"

GdbResponseListener::GdbResponseListener(QObject* parent)
	: QObject(parent)
{
}

GdbResponseListener::~GdbResponseListener()
{
}

void GdbResponseListener::onGdbResponse(const GdbResponse* response, int& maxDuration)
{
	Q_ASSERT(response);
	switch ( response->getType() )
	{
		case GdbResponse::EXEC_ASYNC_OUTPUT: return onExecAsyncOut(response->getItemTree(), response->getReason(), maxDuration);
		case GdbResponse::STATUS_ASYNC_OUTPUT: return onStatusAsyncOut(response->getItemTree(), response->getReason(), maxDuration);
		case GdbResponse::NOTIFY_ASYNC_OUTPUT: return onNotifyAsyncOut(response->getItemTree(), response->getReason(), maxDuration);
		case GdbResponse::LOG_STREAM_OUTPUT: return onLogStreamOutput(response->getText(), maxDuration);
		case GdbResponse::TARGET_STREAM_OUTPUT: return onTargetStreamOutput(response->getText(), maxDuration);
		case GdbResponse::CONSOLE_STREAM_OUTPUT: return onConsoleStreamOutput(response->getText(), maxDuration);
		case GdbResponse::RESULT: return onResult(response->getItemTree(), maxDuration);

		default: break;
	}
}

void GdbResponseListener::onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(asyncClass);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(asyncClass);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(asyncClass);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onResult(const GdbItemTree& tree, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onConsoleStreamOutput(const QString& text, int& maxDuration)
{
	Q_UNUSED(text);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onTargetStreamOutput(const QString& str, int& maxDuration)
{
	Q_UNUSED(str);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onLogStreamOutput(const QString& str, int& maxDuration)
{
	Q_UNUSED(str);
	Q_UNUSED(maxDuration);
}
