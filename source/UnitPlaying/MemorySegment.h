#ifndef MEMORYSEGMENT_H
#define MEMORYSEGMENT_H

#include "LinearLayout.h"

class Scene;
class Unit;

class MemorySegment : public LinearLayout
{
	Q_DISABLE_COPY(MemorySegment)

  protected:
	/// This object represents a memory segement of this playing unit
	Unit& unit;
	/// To reparent children to this scene
	Scene* scene;

  public:
	/// Constructor
	explicit MemorySegment(Unit& unit, Scene* scene, Qt::Orientation orientation);
	/// Destructor
	~MemorySegment();
};

#endif // MEMORYSEGMENT_H
