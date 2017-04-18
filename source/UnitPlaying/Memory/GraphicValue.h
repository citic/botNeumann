#ifndef GRAPHICVALUE_H
#define GRAPHICVALUE_H

#include "Common.h"
#include "LinearLayoutActor.h"
#include "MemoryAllocation.h"

class LabelButton;
class Actor;
class QGraphicsItem;

// Extra z-values for the parts of the graphic variable
const qreal zPodOffset = 0.00002;
const qreal zLabelValueOffset = 0.00004;

/**
	Represents a value on the screen. Values can be standalone. They may be not values of a variable
	For example, characters coming by the standard input are standalone values, not variable values.

	GraphicValue can travel by the scene, moving from one segment to another. GraphicVariable are
	attached to a fixed memory address, i.e. to a memory segment. GraphicVariables have name, but
	GraphicValues do not.
*/
class GraphicValue : public LinearLayoutActor
{
	Q_OBJECT
	Q_DISABLE_COPY(GraphicValue)

  protected:
	/// Data type of this value
	DataType dataType = typeUnknown;
	/// The scene will be the parent for the props used to represent this value, because values
	/// can "float" along the entire scene
	QGraphicsItem* graphicsParent = nullptr;
	/// Z Index where the value should be placed
	qreal zValue = 0.0;
	/// For pointers, the left part of the pod, may be changed for valid or invalid
	Actor* podLeft = nullptr;
	Actor* podMiddle = nullptr;
	Actor* podRight = nullptr;
	/// A label to show the value of the variable
	LabelButton* valueLabel = nullptr;
	/// The label represented as a string
	QString value;

  public:
	/// Constructor
	/// @remarks Client class must call @a buildGraphicValue in order to create the graphics
	/// according to the data type
	explicit GraphicValue(DataType dataType, QGraphicsItem* graphicsParent, qreal zValue, const QString& value = "");
	/// Get the size in bytes of this value
	virtual VisAddress getSize() const;
	/// Set the value and update the interface
	void setValue(const QString& value);
	/// Animate the value changing
	/// @return The duration of the animation in milliseconds, 0 if not value is shown
	int animateValueChange(const QString& value);
	/// Constructs this value according to its data type
	bool buildGraphicValue();
	/// Updates zValue for children actors
	virtual void setZ(qreal newZValue) override;

  protected:
	/// Create the pod, the value, and the label for a bool or char value
	virtual bool buildSingleByteVariable(const QString& asset, const qreal refDataMargins[]);
	/// Create the pod, the value, and the label for an int or float value
	virtual bool buildMultiByteVariable(const QString& asset, const qreal refDataMargins[]);
	/// Create a recursive array
	virtual bool buildArray();
	/// Create a recursive structure
	virtual bool buildStruct();
	/// Apply margins to this graphical object according to the data type
	virtual void applyDataTypeMargins(const qreal refDataMargins[]);
	/// Builds the pod. For some variables the pod may be incomplete (sliced through several
	/// memory rows)
	bool buildPod(const QString& asset, bool buildLeftPod, bool buildRightPod);
	/// Builds a label to show the value (or variable value)
	bool buildValueLabel(const qreal refDataMargins[], qreal proportion = 1.0 - 0.2);
	/// In character values, convert some invisible chacters by scape sequences
	QString processInvisibleChars();
};

#endif // GRAPHICVALUE_H
