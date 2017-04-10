#ifndef DATASEGMENT_H
#define DATASEGMENT_H

#include "GdbResponseListener.h"
#include "MemorySegment.h"

class ExecutionThread;
class MemoryFrame;
class PlayerSolution;
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
	/// Allocate a global or static variable in the first empty space. Notice that data segment
	/// only allows allocation, not deallocation of variables.
	/// @return true if the variable was allocated, false if there is no room for the variable.
	/// In the last case, a segment overflow should be animated
	/// @remarks Deallocation is not allowed in data segment. If @a deallocate() method is called,
	/// botNeumann will be stopped
	virtual bool allocate(MemoryAllocation* memoryAllocation) override;
	/// Loads the expected input/output/error in the StandardInputOutput tubes from the test
	/// case files. It animates characters arriving by the standard input.
	bool loadTestCase(int testCaseNumber, PlayerSolution* playerSolution);

  public slots:
	/// Called when visualization has finished in order to clear variables
	void clearAnimation();
	/// Called when we get an updated cursor from gdb
	/// @see Visualizator::updateStandardInputOutput()
	void updateStandardInputOutput(const GdbItemTree& tree, VisualizationContext context,
		const QList<ExecutionThread*>& inputQueue, const QList<ExecutionThread*>& outputQueue, int& maxDuration);

  protected:
	/// Builds the data segment represented a shared table for all the robots
	void buildDataSegment();
	/// Represent two important variables for I/O: stdin (std::cin) and stdout (std::cout) as
	/// two pipes. They are global variables
	void buildStandardInOut(const double stdInOutProportion);
};

#endif // DATASEGMENT_H
