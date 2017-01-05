#ifndef CPUCORE_H
#define CPUCORE_H

#include "MemorySegment.h"

class CpuCore : public MemorySegment
{
	Q_DISABLE_COPY(CpuCore)

  protected:
	/// Number or index of this CPU core, starting from 0
	int cpuCoreNumber;

  public:
	/// Constructor
	explicit CpuCore(int cpuCoreNumber, Unit& unit, Scene* scene);
	/// Destructor
	virtual ~CpuCore();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const override;

  protected:
	/// Builds the CPU core as a workstation where a execution thread (robot) can work
	void buildCpuCore();
};

#endif // CPUCORE_H
