#ifndef MEMORYALLOCATION_H
#define MEMORYALLOCATION_H

#include <QString>

class GdbCall;
class GdbItemTree;
class GraphicVariable;

/// The data type of the inferior variable
enum DataType
{
	typeUnknown,
	typeVoid,

	// Atomic types
	typeBool,      // bool b
	typeChar,      // char ch, usigned char ch
	typeInt,       // int i, unsigned long long int, volatile signed short
	typeEnum,      // enum { a, b, c }
	typeBitField,  // struct { unsigned f1: 1, unsigned : 0, ... }
	typeFloat,     // float x, double y, long double z

	// Indirection types
	typePointer,   // DataType * ptr, const DataType*** ptr2, DataType[] arr
	typeReference, // DataType& ref
	typeRValue,    // DataType&& ref
	typeFunction,  // DataType func(args) [Not function pointer]

	// Composite types
	typeArray,     // DataType[n] arr
	typeStruct,    // struct { DataType f1; DataType f2; ... }
	typeClass,     // class  { DataType f1; DataType f2; ... }
	typeUnion,     // union  { DataType f1; DataType f2; ... }
};

/** Any piece of memory on the inferior that is mapped to the visualization, e.g: a variable */
struct MemoryAllocation
{
	Q_DISABLE_COPY(MemoryAllocation)

  public:
	/// The type of memory in inferior that this object is mapping
	enum WatchType { watchUnknown, standardInputOutput, globalVariable };

  public:
	/// The type of memory in inferior that this object is mapping
	WatchType watchType = watchUnknown;
	/// Identifier of the variable
	QString name;
	/// Name of the GDB's variable-object used to watch the variable, if any
	QString watchName;
	/// Physical address of the block on inferior, reported by GDB in hexadecimal, 0 means invalid
	size_t inferiorAddress = 0;
	/// Address where this memory block was allocated in visualization, 0 means invalid
	size_t visualizationAddress = 0;
	/// Size in bytes of the block
	size_t size = 0;
	/// The type of this inferior's variable
	DataType dataType = typeUnknown;
	/// The data type reported by GDB as string
	QString dataTypeStr;
	/// The value represented as text (from GDB output)
	QString value;
	/// Thread that created this memory block
	int thread = 0;
	/// Stack frame index, if this is a local variable
	int stackFrame = 0;
	/// True if this memory block was allocated in dyanamic memory by a direct call from player
	/// solution code, false if it was allocated by a library call
	bool directHeapAllocation = false;
	/// Pointer to the graphical object representing this memory block, nullptr if none
	GraphicVariable* graphicVariable = nullptr;

  public:
	/// Convenience constructor
	explicit MemoryAllocation(WatchType type = watchUnknown) : watchType(type) { }
	/// Create a MemoryAllocation from a GDB variable object result
	bool loadFromGdbVariableObject(const GdbItemTree& tree);
	/// Get the name to use to find this object in the hash
	inline const QString& getId() const { return watchName.isEmpty() ? name : watchName; }
	/// Request data to GDB in order to get the minimum missing information to display this variable
	/// in visualization, for example: data size, address, data type
	bool updateMissingFields(GdbCall* debuggerCall);

  protected:
	/// Parses the @a dataTypeStr text trying to identify the data type of the variable
	/// The fields are updated acording to the result (i.e: @a dataType member)
	/// @return true if the data type was recognized, false otherwise
	bool parseDataTypeStr();
	/// @return true if @a dataTypeStr has an atomic data type declaration
	bool parseAtomicDataTypeStr();
	/// @return true if @a dataTypeStr has a pointer or reference declaration
	bool parseIndirectionDataTypeStr();
	/// @return true if @a dataTypeStr has an array declaration
	bool parseArrayDataTypeStr();
	/// @return true if @a dataTypeStr has a struct, class or union declaration
	bool parseCompositeDataTypeStr();
};

#endif // MEMORYALLOCATION_H
