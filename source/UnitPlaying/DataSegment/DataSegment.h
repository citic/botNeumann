#ifndef DATASEGMENT_H
#define DATASEGMENT_H

#include "GdbResponseListener.h"
#include "MemorySegment.h"

class MemoryFrame;
class StandardInputOutput;

class DataSegment : public GdbResponseListener, public MemorySegment
{
	Q_OBJECT
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
	explicit DataSegment(Unit& unit, Scene* scene, QObject* parent = nullptr);
	/// Destructor
	virtual ~DataSegment();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const override;

  protected:
	/// Builds the data segment represented a shared table for all the robots
	void buildDataSegment();
	/// Represent two important variables for I/O: stdin (std::cin) and stdout (std::cout) as
	/// two pipes. They are global variables
	void buildStandardInOut(const double stdInOutProportion, const double zStdInOut);
};

#endif // DATASEGMENT_H
