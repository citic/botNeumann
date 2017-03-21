#include "CallStack.h"
#include "MemoryFrame.h"

CallStack::CallStack(qreal zValue, QGraphicsItem* graphicsParentItem)
	: RectLayoutItem(Qt::Vertical, zValue, graphicsParentItem)
{
}
