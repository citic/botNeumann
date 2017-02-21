#include "VariableMapper.h"
#include "GdbItemTree.h"
#include "LogManager.h"


// MemoryBlock class ---------------------------------------------------------------------------

bool MemoryBlock::loadFromGdbVariableObject(const GdbItemTree& tree)
{
	// The tree has the result of creation of a GDB's variable object, eg:

	//	^done,
	//	name="bn_io_stdin",
	//	numchild="1",
	//	value="0x0",
	//	type="char *",
	//	has_more="0"
	name = tree.findNodeTextValue("name");
	value = tree.findNodeTextValue("value");
	dataType = tree.findNodeTextValue("type");
	//numChildren = tree.findNodeTextValue("numchild");
	//hasMore = tree.findNodeTextValue("has_more");

	return ! name.isEmpty();
}


// VariableMapper class ---------------------------------------------------------------------------

VariableMapper::VariableMapper(QObject* parent)
	: QObject(parent)
{
}

VariableMapper::~VariableMapper()
{
	for ( QHash<QString, MemoryBlock*>::Iterator itr = mapNameMemoryBlock.begin(); itr != mapNameMemoryBlock.end(); ++itr )
		delete itr.value();
}

bool VariableMapper::createStandardInputOutputWatch(const GdbItemTree& tree)
{
	// Create a memory block for the watch and load it from the tree
	MemoryBlock* watch = new MemoryBlock(MemoryBlock::standardInputOutput);

	if ( ! watch->loadFromGdbVariableObject(tree) )
	{
		qCCritical(logApplication) << "Invalid variable-object:" << tree.buildDescription();
		delete watch;
		return false;
	}

	// Add the watch to the maps
	mapNameMemoryBlock.insert( watch->name, watch );
//	qCDebug(logApplication) << "Standard input/output watch added:" << watch->name;

	return true;
}
