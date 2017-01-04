#ifndef HEAPSEGMENT_H
#define HEAPSEGMENT_H

#include "MemorySegment.h"

class MemoryFrame;
class Prop;

class HeapSegment : public MemorySegment
{
	Q_DISABLE_COPY(HeapSegment)

  protected:
	/// Manages the rows of memory for the heap segment
	MemoryFrame* memoryFrame = nullptr;
	/// The interface to access the dynamic memory segment
	Prop* heapInterface = nullptr;

  public:
	/// Constructor
	/// @param inUse if false the Heap Segment is not required for the current unit. It will be
	/// hidden to avoid cognitive load
	explicit HeapSegment(Unit& unit, Scene* scene);
	/// Destructor
	~HeapSegment();

  protected:
	/// Builds the segment with space to store variables (shelves)
	void buildSegment();
	/// Closes the doors of the segment, because it is not used by the current unit
	/// It will be hidden to avoid cognitive load
	void hideSegment();
};

#endif // HEAPSEGMENT_H
