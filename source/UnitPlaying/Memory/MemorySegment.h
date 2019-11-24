#ifndef MEMORYSEGMENT_H
#define MEMORYSEGMENT_H

#include "LinearLayout.h"

struct MemoryAllocation;
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
	virtual ~MemorySegment() override;
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	/// The height of segments or frames is calculated as the number of memory rows they occupy.
	/// Some other elements such as heap interface or standard input/output graphic elements are
	/// calculated as relative to a memory row. The number of rows required by an object can be
	/// calculated from the @a Unit. Then the height of the scene is distributed proportionally
	/// between the objects according to the number of memory rows they require.
	virtual double getHeightInRows() const { return 0; }
	/// Memory segments are able to allocate variables, unless they are read-only (eg: code segment)
	/// @return true if the variable was allocated, false if there are not enough memory and a
	/// segment overflow should be animated
	/// @remarks The default implementation of this method is to stop botNeumann. If an inherited
	/// class is a read-write segment, it must override this method
	virtual bool allocate(MemoryAllocation* memoryAllocation);
	/// Read-write segments are able to deallocate variables
	/// @return true if the given memory allocation (variable) was deallocated, false if the
	/// variable does not exist
	/// @remarks The default implementation of this method is to stop botNeumann. If an inherited
	/// class is a read-write segment that allows deallocation, it must override this method
	virtual bool deallocate(MemoryAllocation* memoryAllocation);
};

#endif // MEMORYSEGMENT_H
