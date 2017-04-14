#ifndef GRAPHICCHARVALUE_H
#define GRAPHICCHARVALUE_H

#include "GraphicValue.h"

/** A GraphicCharValue is a GraphicValue that can travel by any standard input/output tube,
	exit from or enter to it when it arrives to the open tube's area, travel by the scene towars/
	from any execution thread, and convert to other value.

	This complex animation is implemented by this class using a state machine.
*/
class GraphicCharValue : public GraphicValue
{
	Q_OBJECT
	Q_DISABLE_COPY(GraphicCharValue)

  public:
	/// Constructor
	GraphicCharValue(QGraphicsItem* graphicsParent, qreal zValue, const QString& value);
};

#endif // GRAPHICCHARVALUE_H
