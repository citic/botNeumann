#ifndef MEMORYFRAME_H
#define MEMORYFRAME_H

#include "LinearLayout.h"

struct MemoryAllocation;
class MemoryRow;
class Scene;

/** A memory frame is graphical object to represent a piece of computer memory sliced into
	one or more memory rows, covered by a roof row. A label can be placed on the roof.
**/
class MemoryFrame : public LinearLayout
{
	Q_DISABLE_COPY(MemoryFrame)

  protected:
	/// To reparent children to this scene
	Scene* scene;
	/// The number of memory rows this frame has
	size_t rowCount;
	/// The memory address where the first byte of this memory frame starts
	size_t startByte;
	/// The size in bytes of each memory row
	size_t rowSize;
	/// The list of memory rows to show in this frame
	QList<MemoryRow*> memoryRows;

  public:
	/// Constructor
	explicit MemoryFrame(Scene* scene, size_t rowCount, size_t startByte, size_t rowSize, qreal zValue, bool withGarbage);
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const;
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

  protected:
	/// Create the memory rows and place them into the scene
	void buildMemoryFrame(qreal zValue, bool withGarbage);
};

#endif // MEMORYFRAME_H
