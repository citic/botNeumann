#ifndef MEMORYTOP_H
#define MEMORYTOP_H

#include "LinearLayout.h"

class MultiSvgButton;
class QGraphicsItem;

/// Draws a memory roof on top of the memory rows
class MemoryTop : public LinearLayout
{
	Q_DISABLE_COPY(MemoryTop)

  protected:
	/// This is the size of the memory row
	size_t size;
	/// To reparent children
	QGraphicsItem* graphicsParentItem = nullptr;
	/// The z-value for child graphic elements
	qreal zValue;
	/// The label to identify the entire memory frame is placed on the memory top
	MultiSvgButton* label = nullptr;

  public:
	/// Constructor
	MemoryTop(size_t size, const QString& labelText, QGraphicsItem* graphicsParentItem, qreal zValue);

  protected:
	/// Creates the props to display a memory roof
	void buildMemoryTop();
	/// Creates the label for the memory frame
	void buildLabel(const QString& label);
};

#endif // MEMORYTOP_H
