#ifndef MEMORYMAPPER_H
#define MEMORYMAPPER_H

#include "MemoryAllocation.h"

#include <QObject>
#include <QHash>

/** Maps variables in player solution with graphical variables in visualization. Also manages
	some control variables that are used to produce the visualization, but they are not directly
	displayed, e.g: standard input/output watches. */
class MemoryMapper : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MemoryMapper)

  protected:
	/// Pointer to the GDB communication object. The MemoryMapper requires to access GDB in order
	/// to get more information about variables and watches
	GdbCall* debuggerCall = nullptr;
	/// Allows to find memory blocks by their names. Also it owns the memory blocks
	QHash<QString, MemoryAllocation*> mapNameMemoryAllocation;
	/// Allows to find memory blocks by their inferior address
//	QHash<size_t, MemoryAllocation*> mapInferiorAddressMemoryAllocation;

  public:
	/// Constructor
	explicit MemoryMapper(GdbCall* debuggerCall, QObject* parent = nullptr);
	/// Destructor
	~MemoryMapper();
	/// Create a watch object, that is, a variable object in GDB that watches for some other
	/// variable in player solution. When that value changes, we can animate the envolved variable
	/// @param name Original name of the variable being watched in player solution
	/// @param watchName Name of the variable object used to watch the @a name variable
	/// @return true if the watch object was created and added to the hash table
	bool createWatch(const QString& name, const QString& watchName, AllocationSegment segment);
};

#endif // MEMORYMAPPER_H
