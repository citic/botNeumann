#ifndef HEAPSEGMENT_H
#define HEAPSEGMENT_H

#include "GdbResponseListener.h"
#include "MemorySegment.h"

class MemoryFrame;
class Prop;

class HeapSegment : public GdbResponseListener, public MemorySegment
{
	Q_OBJECT
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
	explicit HeapSegment(Unit& unit, Scene* scene, QObject* parent = nullptr);
	/// Destructor
	virtual ~HeapSegment();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const override;
	/// Allocate variables in dynamic memory
	virtual bool allocate(MemoryAllocation* memoryAllocation) override;
	/// Deallocate variables in dynamic memory
	virtual bool deallocate(MemoryAllocation* memoryAllocation) override;

  public slots:
	/// Called when visualization has finished in order to clear variables
	void clearAnimation();

  protected:
	/// Builds the segment with space to store variables (shelves)
	void buildSegment();
	/// Closes the doors of the segment, because it is not used by the current unit
	/// It will be hidden to avoid cognitive load
	void hideSegment();
};

#endif // HEAPSEGMENT_H
