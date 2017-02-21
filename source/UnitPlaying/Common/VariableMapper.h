#ifndef VARIABLEMAPPER_H
#define VARIABLEMAPPER_H

#include <QObject>
#include <QHash>

class GdbItemTree;
class GraphicVariable;

/** Any piece of memory on the inferior that is mapped to the visualization, e.g: a variable */
struct MemoryBlock
{
  public:
	/// The type of memory in inferior that this object is mapping
	enum Type { typeUnknown, standardInputOutput };

  public:
	/// The type of memory in inferior that this object is mapping
	Type type = typeUnknown;
	/// Identifier of the variable
	QString name;
	/// Physical address of the block on inferior, reported by GDB in hexadecimal, 0 means invalid
	size_t inferiorAddress = 0;
	/// Address where this memory block was allocated in visualization, 0 means invalid
	size_t visualizationAddress = 0;
	/// Size in bytes of the block
	size_t size = 0;
	/// The data type reported by GDB as string
	QString dataType;
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
	explicit MemoryBlock(Type type = typeUnknown) : type(type) { }
	/// Create a MemoryBlock from a GDB variable object result
	bool loadFromGdbVariableObject(const GdbItemTree& tree);
};

/** Maps variables in player solution with graphical variables in visualization. Also manages
	some control variables that are used to produce the visualization, but they are not directly
	displayed, e.g: standard input/output watches. */
class VariableMapper : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(VariableMapper)

  protected:
	/// Allows to find memory blocks by their names. Also it owns the memory blocks
	QHash<QString, MemoryBlock*> mapNameMemoryBlock;
	/// Allows to find memory blocks by their inferior address
//	QHash<size_t, MemoryBlock*> mapInferiorAddressMemoryBlock;

  public:
	/// Constructor
	explicit VariableMapper(QObject* parent = nullptr);
	/// Destructor
	~VariableMapper();
	/// Create a watch for standard input/output/error pointer. When this pointer changes, an
	/// input/output operation was done and it should be animated
	/// @return true if the tree was successfully parsed and the watch was created
	bool createStandardInputOutputWatch(const GdbItemTree& tree);
};

#endif // VARIABLEMAPPER_H
