#ifndef MEMORYMAPPER_H
#define MEMORYMAPPER_H

#include "MemoryAllocation.h"

#include <QObject>
#include <QHash>

class HeapSegment;
class CpuCores;
class DataSegment;
class CodeSegment;

/** Maps variables in player solution with graphical variables in visualization. Also manages
	some control variables that are used to produce the visualization, but they are not directly
	displayed, e.g: standard input/output watches. */
class MemoryMapper : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(MemoryMapper)

  protected:
	/// Allows to find memory blocks by their names. Also it owns the memory blocks
	QHash<QString, MemoryAllocation*> mapNameMemoryAllocation;
	/// Allows to find memory blocks by their inferior address
//	QHash<size_t, MemoryAllocation*> mapInferiorAddressMemoryAllocation;

  protected: // Other objects
	/// Keep a pointer to the unique instance of this object while it is alive
	static MemoryMapper* instance;
	/// Pointer to the GDB communication object. The MemoryMapper requires to access GDB in order
	/// to get more information about variables and watches
	GdbCall* debuggerCall = nullptr;
	/// To represent constants (eg: constant variables and functions)
	CodeSegment* codeSegment = nullptr;
	/// The memory mapper should know the segments, in order to alert them when there are changes
	/// in variables, for instance, allocation or deallocation of variables
	/// HeapSegment: to allocate dynamic memory
	HeapSegment* heapSegment = nullptr;
	/// To allocate local variables
	CpuCores* cpuCores = nullptr;
	/// To allocate global and static variables
	DataSegment* dataSegment = nullptr;

  public:
	/// Constructor
	explicit MemoryMapper(GdbCall* debuggerCall, QObject* parent = nullptr);
	/// Destructor
	~MemoryMapper();
	/// Get access to the unique instance of this object while it is alive
	static inline MemoryMapper* getInstance() { return instance; }
	/// Set the CodeSegment for later use
	inline void setCodeSegment(CodeSegment* codeSegment) { this->codeSegment = codeSegment; }
	/// Set the HeapSegment for later use
	inline void setHeapSegment(HeapSegment* heapSegment) { this->heapSegment = heapSegment; }
	/// Set the CpuCores for later use
	inline void setCpuCores(CpuCores* cpuCores) { this->cpuCores = cpuCores; }
	/// Set the DataSegment for later use
	inline void setDataSegment(DataSegment* dataSegment) { this->dataSegment = dataSegment; }
	/// Create a watch object, that is, a variable object in GDB that watches for some other
	/// variable in player solution. When that value changes, we can animate the envolved variable
	/// @param name Original name of the variable being watched in player solution
	/// @param watchName Name of the variable object used to watch the @a name variable
	/// @param shouldAllocate If true, the watched object is allocated in @a segment. When a watch
	/// is allocated, a visualization address is assigned and it appears in the visualization
	/// @return A pointer to the watch object created and added to the hash table, nullptr on error
	MemoryAllocation* createWatch(const QString& name, const QString& watchName, AllocationSegment segment, bool shouldAllocate);
	/// Allocate the given variable in the respective segment
	/// @return true if the variable was allocated in the segment, false if there is no enough space
	/// and a segment overflow should be animated
	bool allocate(MemoryAllocation* memoryAllocation);
};

#endif // MEMORYMAPPER_H
