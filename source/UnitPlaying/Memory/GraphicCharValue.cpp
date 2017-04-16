#include "GraphicCharValue.h"

GraphicCharValue::GraphicCharValue(QGraphicsItem* graphicsParent, qreal zValue, const QString& value)
	: GraphicValue(typeChar, graphicsParent, zValue, value)
{
	buildGraphicValue();
}

int GraphicCharValue::animateRead(int index, ExecutionThread* targetThread)
{
	// ToDo: implement the animation state machine
	Q_UNUSED(index);
	Q_UNUSED(targetThread);

	return 0;
}
