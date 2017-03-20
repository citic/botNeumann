#ifndef GRAPHICVARIABLE_H
#define GRAPHICVARIABLE_H

#include "GraphicValue.h"

#include <QList>

struct MemoryAllocation;
class MemoryRow;
class MultiSvgButton;

/** Represents graphically a variable within a memory row, or a segment of the variable if it does
	not fit within the memory row (in such case, the variable spans for several memory rows). */
class GraphicVariable : public GraphicValue
{
	Q_OBJECT
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
	/// A label to show the variable name. Only available in large pods (4 or more bytes)
	MultiSvgButton* nameLabel = nullptr;
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
	virtual VisAddress getSize() const override { return lastByte - firstByte + 1; }

  protected:
	/// Create the pod, the value, and the label for an int or float variable
	virtual bool buildMultiByteVariable(const QString& asset, const qreal refDataMargins[]) override;
	/// Create a recursive array of variables
	virtual bool buildArray() override;
	/// Create a recursive structure of variables
	virtual bool buildStruct() override;
	/// Apply margins to this variable according to the data type. If the variable is child of
	/// other variable (composite data types), minimum margins are applied instead
	virtual void applyDataTypeMargins(const qreal refDataMargins[]) override;
	/// Variables have a name. If there is enough room, name is shown
	bool buildVariableName(const qreal refDataMargins[]);
};

#endif // GRAPHICVARIABLE_H
