#ifndef MEMORYROW_H
#define MEMORYROW_H

#include "Common.h"
#include "LinearLayout.h"

#include <QList>

struct MemoryAllocation;
class GraphicVariable;
class Prop;
class QGraphicsItem;

/**
	@brief A row or line of memory represented as a shelf in botNeumann
 */
class MemoryRow : public LinearLayout
{
	Q_DISABLE_COPY(MemoryRow)

  public:
	/// Memory rows are represented as shelves. These shelves allocate variables (values). Shelves
	/// have a space to labels. We can display on them one of the following: memory addresses (the
	/// default), variable names, variable data types. User can switch the label type anytime s/he
	/// wants
	enum class LabelType
	{
		memoryAddresses,
		variableNames,
		dataTypes
	};

  protected:
	/// This row starts at this byte, it could be also called offset
	VisAddress start;
	/// This is the size of the memory row
	VisAddress size;
	/// To reparent children to this scene
	QGraphicsItem* graphicsParentItem = nullptr;
	/// The type of lables to show under each shelf
	LabelType labelType;
	/// The z-index where this memory row is shown
	qreal zValue = 0.0;
	/// If this memory row can have uninitiallized variables, we create random artifacts (garbage),
	/// one for each byte. These garbage can be shown or hidden later, according to the variables
	/// allocated, deallocated, or the free space changes.
	QList<Prop*> garbage;
	/// The list of variables that are allocated in this memory row
	QList<GraphicVariable*> graphicVariables;

  public:
	/// Constructor
	explicit MemoryRow(VisAddress start, VisAddress size, QGraphicsItem* graphicsParentItem, qreal zValue, bool withGarbage);
	/// Destructor
	~MemoryRow();
	/// Get access to the scene. Required by GraphicVariables
	inline QGraphicsItem* getGraphicsParentItem() const { return graphicsParentItem; }
	/// Get access to the z-value where this memory row is. GraphicVariables require this value also
	/// to be displayed at the same z-value than the memory row
	inline qreal getZValue() const { return zValue; }
	/// Display memory addresses on labels
	void displayMemoryAddresses();
	/// Display variable names on labels
	void displayVariableNames();
	/// Display data types on labels
	void displayDataTypes();
	/// Cycle between label types
	void cycleLabelType();
	/// Allocate the given variable in this memory row if the variable is within the range of this
	/// memory row.
	/// @return true if the variable is entirely allocated, false if the variable is not
	/// allocated at all or there is pending bytes of the variable to be allocated in the next
	/// memory row
	bool allocate(MemoryAllocation* variable);
	/// Shows garbage in the given range of bytes
	/// @return true on success, false if the range is not valid for this memory row
	bool showGarbage(VisAddress firstByte, VisAddress lastByte, bool visible = true);
	/// Convenience function to hide a range of garbage artifacts
	inline bool hideGarbage(VisAddress firstByte, VisAddress lastByte)
		{ return showGarbage(firstByte, lastByte, false); }
	/// Deallocate all variable fragmets in this memory row. They may be replaced by garbage if
	/// this is a part of a memory segment that is not initiallized
	bool deallocateAll();

  protected:
	/// Build the shelves and labels
	void buildMemoryRow(bool withGarbage);
	/// Build labels for memory addresses
	void buildMemoryAddresses();
	/// Fills this memory row with garbage in each byte
	void buildGarbage();
	/// Calculates the horizontal proportion of a byte in this memory row
	inline qreal getByteProportion() const { return 1.0 / (size + 2.0); }
	/// Calculate the intersection between the addresses of the variable and the addresses of this
	/// memory row.@return true if there is intersection, false if intersection is empty
	bool calculateIntersection(const MemoryAllocation* variable, VisAddress& firstByte, VisAddress& lastByte);
};

#endif // MEMORYROW_H
