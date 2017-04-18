#include "GraphicCharValue.h"
#include "ExecutionThread.h"
#include "Scene.h"

GraphicCharValue::GraphicCharValue(QGraphicsItem* graphicsParent, qreal zValue, const QString& value)
	: GraphicValue(typeChar, graphicsParent, zValue, value)
{
	buildGraphicValue();
}

int GraphicCharValue::animateRead(int index, int length, int ioBufferCapacity, ExecutionThread* targetThread, Scene* scene)
{
	// If this chacter is hidden, it will have to wait to appear in the stdin tube
	int duration = 0;
	if ( index > ioBufferCapacity )
		duration += (index - ioBufferCapacity) * 250;

	// If this character was not read, it will be only moved through the stdin tube
	if ( index >= length )
	{
		// To animate them arriving, we place them at the first non visible position
		qreal finalPercent = qreal( index - length ) / ioBufferCapacity;
		duration += animateMoveTo( finalPercent, (index - length) * 250, duration );
		return duration;
	}

	// This character

	Q_UNUSED(targetThread);
	Q_UNUSED(scene);

	return duration;
}
