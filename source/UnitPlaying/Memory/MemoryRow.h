#ifndef MEMORYROW_H
#define MEMORYROW_H

#include "Common.h"
#include "LinearLayout.h"

#include <QList>

struct MemoryAllocation;
class Actor;
class GraphicVariable;
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
	QList<Actor*> garbage;
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
	/// Return the first byte after the last used by this row
	inline size_t getLastByte() const { return start + size; }
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
	/// @param entirelyAllocated This param will be set 1 if the variable was entirely allocated,
	/// 0 if it was partially allocated, and -1 if the variable was not allocated at all. In cases
	/// 0 or negative, a new memory row should be used to allocated the remaining of the variable
	/// @return The duration of the animation in milliseconds if the variable is allocated, or
	/// a negative integer if the variable was not allocated at all
	int allocate(MemoryAllocation* variable, int& entirelyAllocated, int initialDelay);
	/// Shows garbage in the given range of bytes
	/// @return The duration in milliseconds of the animation (showing or hiding garbabe).
	/// Return 0 if this row does not have gabage. A negative integer if the given range is not
	/// valid for this memory row
	int showGarbage(VisAddress firstByte, VisAddress lastByte, bool visible, int initialDelay, int& entirelyAllocated);
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
