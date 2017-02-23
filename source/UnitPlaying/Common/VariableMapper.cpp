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
	watchName = tree.findNodeTextValue("name");
	value = tree.findNodeTextValue("value");
	dataType = tree.findNodeTextValue("type");
	//numChildren = tree.findNodeTextValue("numchild");
	//hasMore = tree.findNodeTextValue("has_more");

	return ! watchName.isEmpty();
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

bool VariableMapper::createWatch(const QString& name, const QString& watchName, MemoryBlock::Type type)
{
	// Create a memory block for the watch and load it from the tree
	MemoryBlock* watch = new MemoryBlock(type);
	watch->name = name;
	watch->watchName = watchName;

	// Add the watch to the maps
	mapNameMemoryBlock.insert( watch->getId(), watch );
	qCDebug(logApplication) << "Watch created:" << watch->getId();

	return true;
}

bool VariableMapper::updateWatch(const GdbItemTree& tree)
{
	// Find the watch in the hash to update it
	const QString& watchName = tree.findNodeTextValue("name");
	if ( ! mapNameMemoryBlock.contains(watchName) )
	{
		qCCritical(logApplication) << "Variable-object not found in MemoryMapper:" << watchName;
		return false;
	}

	MemoryBlock* watch = mapNameMemoryBlock.value( watchName );
	Q_ASSERT(watch);

	// Update the watch from GDB's tree result
	if ( ! watch->loadFromGdbVariableObject(tree) )
	{
		qCCritical(logApplication) << "Invalid watch tree:" << tree.buildDescription();
		return false;
	}

	qCDebug(logApplication) << "Watch updated:" << watch->getId() << "referring to" << watch->name;
	return true;
}
