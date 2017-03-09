#ifndef GRAPHICVARIABLE_H
#define GRAPHICVARIABLE_H

#include "Common.h"
#include "LinearLayout.h"

#include <QList>

class LabelButton;
struct MemoryAllocation;
class MemoryRow;
class ScenicElement;
class SvgButton;

/** Represents graphically a variable within a memory row, or a segment of the variable if it does
	not fit within the memory row (in such case, the variable spans for several memory rows). */
class GraphicVariable : public LinearLayout
{
	Q_DISABLE_COPY(GraphicVariable)

  protected:
	/// The variable being represented
	MemoryAllocation* variable = nullptr;
	/// The first byte on the row where this variable starts
	VisAddress firstByte = 0;
	/// The last byte on the row occupied by this variable
	VisAddress lastByte = 0;
	/// True if this graphic variable has the first byte of the variable
	bool leftComplete = false;
	/// True if this graphic variable has the last byte of the variable
	bool rightComplete = false;
	/// The memory row where this graphic variable is placed
	MemoryRow* memoryRow = nullptr;
	/// For pointers, the left part of the pod, may be changed for valid or invalid
	ScenicElement* podLeft = nullptr;
	/// A label to show the variable name. Only available in large pods (4 or more bytes)
	SvgButton* label = nullptr;
	/// A label to show the value of the variable
	LabelButton* value = nullptr;
	/// Composite types are recursive, therefore represented as a tree
	GraphicVariable* parent = nullptr;
	/// Elements of arrays, or members of structures
	QList<GraphicVariable*> children;

  public:
	/// Constructor
	GraphicVariable(MemoryAllocation* variable, VisAddress firstByte, VisAddress lastByte, MemoryRow* memoryRow);
	/// Returns true if this variable is allocated at the given address
	inline bool isAllocatedAt(VisAddress firstByte, VisAddress lastByte) const
		{ return this->firstByte == firstByte && this->lastByte == lastByte; }
	/// Calculates the size of this variable's fragment
	inline VisAddress getSize() const { return lastByte - firstByte + 1; }

  protected:
	/// Constructs this variable fragment
	bool buildGraphicVariable();
	/// Create the pod, the value, and the label for a bool or char variable
	bool buildSingleByteVariable(const QString& asset, const qreal dataMarginTop, const qreal dataMarginBottom);
	/// Create the pod, the value, and the label for an int or float variable
	bool buildMultiByteVariable(const QString& asset, const qreal dataMarginTop, const qreal dataMarginBottom);
	/// Create a recursive array of variables
	bool buildArray();
	/// Create a recursive structure of variables
	bool buildStruct();
	/// Apply margins to this variable according to the data type. If the variable is child of
	/// other variable (composite data types), minimum margins are applied instead
	void applyDataTypeMargins(const qreal dataMarginTop, const qreal dataMarginBottom);
};

#endif // GRAPHICVARIABLE_H
