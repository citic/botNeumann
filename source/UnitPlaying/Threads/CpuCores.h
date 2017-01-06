#ifndef CPUCORES_H
#define CPUCORES_H

#include "GdbResponseListener.h"
#include "MemorySegment.h"

#include <QList>

class CpuCore;

class CpuCores : public GdbResponseListener, public MemorySegment
{
	Q_OBJECT
	Q_DISABLE_COPY(CpuCores)

  protected:
	/// Available CPU cores to have execution threads (robots) working
	QList<CpuCore*> cpuCores;

  public:
	/// Constructor
	explicit CpuCores(Unit& unit, Scene* scene, QObject* parent = nullptr);
	/// Destructor
	virtual ~CpuCores();
	/// Get the number of memory rows required by this object
	/// @see MemorySegment::getHeightInRows()
	virtual double getHeightInRows() const override;

  protected:
	/// Creates a CPU core (workstation) for each CPU core requested in .botnu file
	void createCpuCores();
	/// Notifications that begin with '=', for example '=thread-group-added,id="id"'
	///	@see GdbResponseListener::onNotifyAsyncOut()
	virtual void onNotifyAsyncOut(const GdbItemTree& tree, AsyncClass asyncClass, int& maxDuration) override;
};

#endif // CPUCORES_H
