#ifndef CPUCORES_H
#define CPUCORES_H

#include "MemorySegment.h"
#include <QList>

class CpuCore;

class CpuCores : public MemorySegment
{
	Q_DISABLE_COPY(CpuCores)

  protected:
	/// Available CPU cores to have execution threads (robots) working
	QList<CpuCore*> cpuCores;

  public:
	/// Constructor
	explicit CpuCores(Unit& unit, Scene* scene);
	/// Destructor
	virtual ~CpuCores();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const override;

  protected:
	/// Creates a CPU core (workstation) for each CPU core requested in .botnu file
	void createCpuCores();
};

#endif // CPUCORES_H
