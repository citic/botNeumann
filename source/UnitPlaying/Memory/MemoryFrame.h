#ifndef MEMORYFRAME_H
#define MEMORYFRAME_H

#include "Common.h"
#include "LinearLayout.h"

#include <QLinkedList>

struct MemoryAllocation;
class MemoryRow;
class MemoryTop;
class QGraphicsItem;

typedef QLinkedList<MemoryAllocation*> MemoryAllocations;

/** A memory frame is graphical object to represent a piece of computer memory sliced into
	one or more memory rows, covered by a roof row. A label can be placed on the roof.
**/
class MemoryFrame : public LinearLayoutActor
{
	Q_OBJECT
	Q_DISABLE_COPY(MemoryFrame)

  protected:
	/// To reparent children
	QGraphicsItem* graphicsParentItem = nullptr;
	/// The number of memory rows this frame has
	size_t rowCount;
	/// The memory address where the first byte of this memory frame starts
	size_t startByte;
	/// The size in bytes of each memory row
	size_t rowSize;
	/// The top row of the memory frame, that can contain a name for the frame
	MemoryTop* memoryTop = nullptr;
	/// The list of memory rows to show in this frame
	QList<MemoryRow*> memoryRows;
	/// The list of variables assigned in this memory frame, some of the may be free space
	MemoryAllocations memoryAllocations;
	/// True if this memory fame has legs at the bottom
	bool withLegs = false;

  public:
	/// Constructor
	explicit MemoryFrame(QGraphicsItem* graphicsParentItem, size_t rowCount, size_t startByte, size_t rowSize, const QString& topLabel, qreal zValue, bool withGarbage, bool withLegs);
	/// Destructor
	~MemoryFrame();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const;
	/// Calculates the size in bytes of this MemoryFrame
	inline size_t getSize() const { return rowSize * rowCount; }
	/// Allocate the given piece of memory (usually a variable) in this memory frame. If there is
	/// enough memory the variable will appear in some memory rows in the visualization. The
	/// the visualizationAddress will be updated in the @a memoryAllocation object.
	/// @return true, if the allocation was done successfully, false if there is not enough space
	/// to allocate the piece of memory (eg: frame is full, or there is free fragments but none is
	/// larger than memoryAllocation.size). In that case, the caller should animate a segment
	/// overflow and stop the animation
	bool allocate(MemoryAllocation* memoryAllocation);
	/// Deallocate the given piece of memory (usually a variable). The memoryAllocation object
	/// will be removed from the scene, but not deleted from memory. The memoryMapper should do
	/// the deletion of the object
	bool deallocate(MemoryAllocation* memoryAllocation);
	/// Deallocate all variables in this frame
	/// @todo: Receive duration by parameter and return actual duration
	bool deallocateAll();

  protected:
	/// Create the memory rows and place them into the scene
	void buildMemoryFrame(const QString& topLabel, qreal zValue);
	/// Create the memory legs, if asked
	void buildMemoryLegs(qreal zValue);
	/// Finds the smallest free fragment to allocate a variable
	/// If a free fragment is found, retuns true and the parameters are assigned to the free
	/// fragment (iterator) and the offset in that iterator. If no fragment is found, returns false
	/// and the parameters are set to invalid iterators and a negative offset
	bool findSmallestFreeFragmentToAllocate(const MemoryAllocation* variable, MemoryAllocations::iterator& smallestFragment, VisAddress& offset);
	/// Distribute the variables allocated in this memory frame to the respective memory rows
	/// @return true if all variables were set, false if there is a segment overflow
	bool distributeVariablesIntoMemoryRows();
	/// Remove free fragments from memory
	void removeMemoryAllocations();
	/// For debugging purposes
	void printAllocationQueue();
};

#endif // MEMORYFRAME_H
