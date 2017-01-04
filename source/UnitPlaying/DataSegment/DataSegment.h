#ifndef DATASEGMENT_H
#define DATASEGMENT_H

#include "MemorySegment.h"

class MemoryFrame;
class StandardInputOutput;

class DataSegment : public MemorySegment
{
	Q_DISABLE_COPY(DataSegment)

  protected:
	/// Manages the rows of memory for the data segment
	MemoryFrame* memoryFrame = nullptr;
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
	void buildStandardInOut(const double stdInOutProportion, const double zStdInOut);
};

#endif // DATASEGMENT_H
