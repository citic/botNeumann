#ifndef MEMORYTOP_H
#define MEMORYTOP_H

#include "LinearLayout.h"

class Scene;

/// Draws a memory roof on top of the memory rows
class MemoryTop : public LinearLayout
{
  protected:
	/// This is the size of the memory row
	size_t size;
	/// To reparent children to this scene
	Scene* scene;

  public:
	/// Constructor
	MemoryTop(size_t size, Scene* scene);

  protected:
	/// Creates the props to display a memory roof
	void buildMemoryTop();
};

#endif // MEMORYTOP_H
