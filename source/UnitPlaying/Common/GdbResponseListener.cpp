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
	VisualizatorContext context = static_cast<VisualizatorContext>( response->getUserData() );
	switch ( response->getType() )
	{
		case GdbResponse::EXEC_ASYNC_OUTPUT: return onExecAsyncOut(response->getItemTree(), response->getReason(), context, maxDuration);
		case GdbResponse::STATUS_ASYNC_OUTPUT: return onStatusAsyncOut(response->getItemTree(), response->getReason(), context, maxDuration);
		case GdbResponse::NOTIFY_ASYNC_OUTPUT: return onNotifyAsyncOut(response->getItemTree(), response->getReason(), context, maxDuration);
		case GdbResponse::LOG_STREAM_OUTPUT: return onLogStreamOutput(response->getText(), context, maxDuration);
		case GdbResponse::TARGET_STREAM_OUTPUT: return onTargetStreamOutput(response->getText(), context, maxDuration);
		case GdbResponse::CONSOLE_STREAM_OUTPUT: return onConsoleStreamOutput(response->getText(), context, maxDuration);
		case GdbResponse::RESULT: return onResult(response->getItemTree(), context, maxDuration);

		default: break;
	}
}

void GdbResponseListener::onExecAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(asyncClass);
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onStatusAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(asyncClass);
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(asyncClass);
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onResult(const GdbItemTree& tree, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(tree);
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onConsoleStreamOutput(const QString& text, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(text);
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onTargetStreamOutput(const QString& str, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(str);
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
}

void GdbResponseListener::onLogStreamOutput(const QString& str, VisualizatorContext context, int& maxDuration)
{
	Q_UNUSED(str);
	Q_UNUSED(context);
	Q_UNUSED(maxDuration);
}
