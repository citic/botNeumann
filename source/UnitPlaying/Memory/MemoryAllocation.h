#ifndef MEMORYALLOCATION_H
#define MEMORYALLOCATION_H

#include <QList>
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

/// For integral types (int), its size can be adjusted
enum SizeQualifier
{
	sizeShort,
	sizeDefault,
	sizeLong,
	sizeLongLong
};


/** Any piece of memory on the inferior that is mapped to the visualization, e.g: a variable */
struct MemoryAllocation
{
	Q_DISABLE_COPY(MemoryAllocation)

  public:
	/// The type of memory in inferior that this object is mapping
	enum WatchType { watchUnknown, standardInputOutput, globalVariable, compoundType };

  public: // Mapping properties
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
	/// The unrolled data type reported by GDB as string. An unrolled type results of converting
	/// typedefs and templates in built-in types
	QString unrolledDataTypeStr;
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

  public: // Atomic value
	/// True if the value is const. For a pointer, true if the address stored in value cannot change
	bool isConst = false;
	/// True if the atomic value is volatile
	bool isVolatile = false;
	/// For integral types, true if the value has sign, false otherwise
	bool isSigned = true;
	/// For integral types (int), its size can be adjusted
	SizeQualifier sizeQualifier = sizeDefault;

  public: // Indirection and composite types
	/// Composite types are recursive, therefore represented as a tree
	MemoryAllocation* parent = nullptr;
	/// For pointers and references, children[0] is the pointed data
	QList<MemoryAllocation*> children;

  public:
	/// Convenience constructor
	explicit MemoryAllocation(WatchType type = watchUnknown) : watchType(type) { }
	/// Destructor
	~MemoryAllocation();
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
	bool parseDataTypeStr(const QString& text, GdbCall* debuggerCall);
	/// @return true if @a dataTypeStr has an atomic data type declaration
	bool parseAtomicDataTypeStr(const QString& text);
	/// Converts a text to the enumeration type
	static SizeQualifier mapSizeQualifier(const QString& text);
	/// Converts a text to the enumeration type
	static DataType mapDataType(const QString& text);
	/// @return true if @a dataTypeStr has a pointer or reference declaration
	bool parseIndirectionDataTypeStr(const QString& text);
	/// @return true if @a dataTypeStr has an array declaration
	bool parseArrayDataTypeStr(const QString& text, GdbCall* debuggerCall);
	/// @return true if @a dataTypeStr has a struct, class or union declaration
	bool parseCompositeDataTypeStr(const QString& text);
};

#endif // MEMORYALLOCATION_H
