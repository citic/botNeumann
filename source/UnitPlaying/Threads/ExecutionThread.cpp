#include "ExecutionThread.h"

ExecutionThread::ExecutionThread(int id, QGraphicsItem* parentItem)
	: Actor(QString(":/unit_playing/threads/thread%1_back.svg").arg( (id - 1) % 4 + 1 ), parentItem)
	, id(id)
{
}
