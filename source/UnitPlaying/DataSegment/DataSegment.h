#ifndef DATASEGMENT_H
#define DATASEGMENT_H

#include "MemorySegment.h"

class StandardInputOutput;

class DataSegment : public MemorySegment
{
	Q_DISABLE_COPY(DataSegment)

  protected:
	/// Standard input graphic manager
	StandardInputOutput* standardInput = nullptr;
	/// Standard output graphic manager
	StandardInputOutput* standardOutput = nullptr;

  public:
	/// Constructor
	explicit DataSegment(Unit& unit, Scene* scene);
	/// Destructor
	~DataSegment();

  protected:
	/// Builds the data segment represented a shared table for all the robots
	void buildDataSegment();
	/// Represent two important variables for I/O: stdin (std::cin) and stdout (std::cout) as
	/// two pipes. They are global variables
	void buildStandardInOut(LinearLayout* contentsLayout, const double stdInOutProportion, const double zStdInOut);
};

#endif // DATASEGMENT_H
