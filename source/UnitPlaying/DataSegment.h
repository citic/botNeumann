#ifndef DATASEGMENT_H
#define DATASEGMENT_H

#include "LinearLayout.h"

class Scene;

class DataSegment : public LinearLayout
{
	Q_DISABLE_COPY(DataSegment)

  protected:
	/// Size in bytes of this segment. It is derived from the .botnu unit
	size_t dataSegmentSize;
	/// To reparent children to this scene
	Scene* scene;

  public:
	/// Constructor
	explicit DataSegment(size_t dataSegmentSize, Scene* scene);
	/// Destructor
	~DataSegment();

  protected:
	/// Builds the data segment represented a shared table for all the robots
	void buildDataSegment();
	/// Represent two important variables for I/O: stdin (std::cin) and stdout (std::cout) as
	/// two pipes. They are global variables
	void buildStandardInOut();
};

#endif // DATASEGMENT_H
