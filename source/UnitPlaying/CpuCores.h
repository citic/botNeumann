#ifndef CPUCORES_H
#define CPUCORES_H

#include "LinearLayout.h"
#include <QList>

class CpuCore;
class Scene;

class CpuCores : public LinearLayout
{
	Q_DISABLE_COPY(CpuCores)

  protected:
	/// Available number of CPU cores (workstations) in botNeumann
	/// This number is suggested by the .botnu file, but the application can override it
	int availableCpuCoreCount;
	/// Mininmun number of required threads to complete the unit
	/// This number is indicated by the .botnu file
	int minThreadCount;
	/// Size in bytes for all CPU cores
	size_t size;
	/// To reparent children to this scene
	Scene* scene;
	/// Available CPU cores to have execution threads (robots) working
	QList<CpuCore*> cpuCores;

  public:
	/// Constructor
	explicit CpuCores(int availableCpuCoreCount, int minThreadCount, size_t size, Scene* scene);
	/// Destructor
	~CpuCores();

  protected:
	/// Creates a CPU core (workstation) for each CPU core requested in .botnu file
	void createCpuCores();
};

#endif // CPUCORES_H
