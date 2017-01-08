#ifndef MEMORYFRAME_H
#define MEMORYFRAME_H

#include "LinearLayout.h"

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
	explicit MemoryFrame(Scene* scene, size_t rowCount, size_t startByte, size_t rowSize, qreal zValue);
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const;

  protected:
	/// Create the memory rows and place them into the scene
	void buildMemoryFrame(qreal zValue);
};

#endif // MEMORYFRAME_H
