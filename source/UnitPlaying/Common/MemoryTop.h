#ifndef MEMORYTOP_H
#define MEMORYTOP_H

#include "LinearLayout.h"

class Scene;

/// Draws a memory roof on top of the memory rows
class MemoryTop : public LinearLayout
{
	Q_DISABLE_COPY(MemoryTop)

  protected:
	/// This is the size of the memory row
	size_t size;
	/// To reparent children to this scene
	Scene* scene;

  public:
	/// Constructor
	MemoryTop(size_t size, Scene* scene, qreal zValue);

  protected:
	/// Creates the props to display a memory roof
	void buildMemoryTop(qreal zValue);
};

#endif // MEMORYTOP_H
