#ifndef CPUCORE_H
#define CPUCORE_H

#include "LinearLayout.h"

class Scene;

class CpuCore : public LinearLayout
{
	Q_DISABLE_COPY(CpuCore)

  protected:
	/// Size in bytes of stack segments that will be hold in this CPU core.
	/// It is derived from the .botnu unit
	size_t stackSegmentSize;
	/// To reparent children to this scene
	Scene* scene;

  public:
	/// Constructor
	explicit CpuCore(size_t stackSegmentSize, Scene* scene);
	/// Destructor
	~CpuCore();

  protected:
	/// Builds the CPU core as a workstation where a execution thread (robot) can work
	void buildCpuCore();
};

#endif // CPUCORE_H
