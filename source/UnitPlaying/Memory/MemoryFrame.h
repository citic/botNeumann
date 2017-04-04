#ifndef MEMORYFRAME_H
#define MEMORYFRAME_H

#include "Common.h"
#include "LinearLayoutActor.h"

#include <QLinkedList>

struct MemoryAllocation;
class MemoryRow;
class MemoryTop;
class QGraphicsItem;

typedef QLinkedList<MemoryAllocation*> MemoryAllocations;

/** A memory frame is graphical object to represent a piece of computer memory sliced into
	one or more memory rows, covered by a roof row. A label can be placed on the roof.

	The maximum size allowed to grow. There are two types of frames. Fixed-size frames have a
	fixed @a rowCount, usually 1 or higher. For example: memory segments. Dynamic-sized frames
	can grow when local variables are declared. These have @a maxSize value higher than 0. For
	example, function calls. The behavior between these two variables is shown in this table:

	   rowCount    maxSize   Behavior
			  0          0   Empty frame. Eg: a library function call
			 1+          0   Fixed-size frame. Eg: data segment
			  0        2^p   Dynamic-sized frame. Eg: function call
			 1+        2^p   Dynamic-sized frame with a known initial size
**/
class MemoryFrame : public LinearLayoutActor
{
	Q_OBJECT
	Q_DISABLE_COPY(MemoryFrame)

  protected:
	/// To reparent children
	QGraphicsItem* graphicsParentItem = nullptr;
	/// For the memory rows
	qreal zValue = 0.0;
	/// The number of memory rows this frame has
	size_t rowCount;
	/// The memory address where the first byte of this memory frame starts
	size_t startByte;
	/// The size in bytes of each memory row
	size_t rowSize;
	/// The maximum size allowed to grow. @see class details
	size_t maxSize;
	/// The top row of the memory frame, that can contain a name for the frame
	MemoryTop* memoryTop = nullptr;
	/// The list of memory rows to show in this frame
	QList<MemoryRow*> memoryRows;
	/// If @a withLegs is true, this points to the legs graphical object
	LinearLayout* legsLayout = nullptr;
	/// The list of variables assigned in this memory frame, some of the may be free space
	MemoryAllocations memoryAllocations;
	/// True if this memory frame contains garbage for uninitiallized variables
	bool withGarbage = false;
	/// True if this memory fame has legs at the bottom
	bool withLegs = false;

  public:
	/// Constructor
	explicit MemoryFrame(QGraphicsItem* graphicsParentItem, size_t rowCount, size_t startByte, size_t rowSize, const QString& topLabel, qreal zValue, bool withGarbage, bool withLegs, size_t maxSize = 0);
	/// Destructor
	~MemoryFrame();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const;
	/// Calculates the size in bytes of this MemoryFrame
	inline size_t getSize() const { return rowSize * rowCount; }
	/// Get the number of rows required by this frame
	inline size_t getRowCount() const { return rowCount; }
	/// Allocate the given piece of memory (usually a variable) in this memory frame. If there is
	/// enough memory the variable will appear in some memory rows in the visualization. The
	/// the visualizationAddress will be updated in the @a memoryAllocation object.
	/// @return duration in milliseconds of the variable-creation animation if the allocation was
	/// done successfully. A negative integer if there is not enough space to allocate the piece of
	/// memory (eg: frame is full, or there is free fragments but none is larger than
	/// memoryAllocation.size). In that case, the caller should animate a segment overflow and stop
	/// the animation
	int allocate(MemoryAllocation* memoryAllocation, int initialDelay);
	/// Deallocate the given piece of memory (usually a variable). The memoryAllocation object
	/// will be removed from the scene, but not deleted from memory. The memoryMapper should do
	/// the deletion of the object
	bool deallocate(MemoryAllocation* memoryAllocation);
	/// Deallocate all variables in this frame
	/// @todo: Receive duration by parameter and return actual duration
	bool deallocateAll();

  protected:
	/// Create the memory rows and place them into the scene
	void buildMemoryFrame(const QString& topLabel);
	/// Create the given amount of memory rows and add them to the scene
	bool createRows(size_t rowCount, size_t fromByte);
	/// Create the memory legs, if asked
	void buildMemoryLegs(qreal zValue);
	/// Finds the smallest free fragment to allocate a variable
	/// If a free fragment is found, retuns true and the parameters are assigned to the free
	/// fragment (iterator) and the offset in that iterator. If no fragment is found, returns false
	/// and the parameters are set to invalid iterators and a negative offset
	bool findSmallestFreeFragmentToAllocate(const MemoryAllocation* variable, MemoryAllocations::iterator& smallestFragment, VisAddress& offset);
	/// Distribute the variables allocated in this memory frame to the respective memory rows
	/// @return the duration of the animation if all variables were set. A negative integer if there
	/// is a segment overflow
	int distributeVariablesIntoMemoryRows(int initialDelay);
	/// Grow this memory frame in the given amount of rows.
	/// @return The duration in milliseconds if the frame grew. A negative integer (-1) if there is
	/// not enough memory to grow
	int grow(int extraRows, int initialDelay);
	/// Adjust the proportions of the rows when the amount of rows have changed (grown)
	void updateRowProportions();
	/// Remove free fragments from memory
	void removeMemoryAllocations();
	/// For debugging purposes
	void printAllocationQueue();
};

#endif // MEMORYFRAME_H
