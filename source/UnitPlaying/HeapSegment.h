#ifndef HEAPSEGMENT_H
#define HEAPSEGMENT_H

#include "LinearLayout.h"

class Scene;

class HeapSegment : public LinearLayout
{
	Q_DISABLE_COPY(HeapSegment)

  protected:
	/// If false the Heap Segment is not required for the current unit. It will be
	/// hidden to avoid cognitive load
	bool inUse;
	/// Size of this Heap Segment in bytes. It is derived from the .botnu unit
	size_t size;
	/// To reparent children to this scene
	Scene* scene;

  public:
	/// Constructor
	/// @param inUse if false the Heap Segment is not required for the current unit. It will be
	/// hidden to avoid cognitive load
	explicit HeapSegment(bool inUse, size_t ramSize, Scene* scene);
	/// Destructor
	~HeapSegment();

  protected:
	/// Builds the segment with space to store variables (shelves)
	void buildSegment();
	/// Closes the doors of the segment, because it is not used by the current unit
	void hideSegment();
};

#endif // HEAPSEGMENT_H
