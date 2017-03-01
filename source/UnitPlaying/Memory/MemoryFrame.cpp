#include "MemoryFrame.h"
#include "LogManager.h"
#include "MemoryAllocation.h"
#include "MemoryRow.h"
#include "MemoryTop.h"
#include "Scene.h"

// The roof requires half memory row
const double memoryRoofRows = 0.5;

MemoryFrame::MemoryFrame(Scene* scene, size_t rowCount, size_t startByte, size_t rowSize, qreal zValue, bool withGarbage)
	: LinearLayout(Qt::Vertical)
	, scene(scene)
	, rowCount(rowCount)
	, startByte(startByte)
	, rowSize(rowSize)
{
	// Initially there is just one big fragment of free memory
	MemoryAllocation* freeFragment = new MemoryAllocation(AllocationSegment::free, getSize(), startByte );
	freeFragment->hasGarbage = withGarbage;
	memoryAllocations.append(freeFragment);
	printAllocationQueue();

	// Draw the memory frame in the visualization
	buildMemoryFrame(zValue);
}

MemoryFrame::~MemoryFrame()
{
	// Free fragments are not added to the scene because they are invisible
	for ( MemoryAllocations::iterator current = memoryAllocations.begin(); current != memoryAllocations.end(); ++current )
		if ( (*current)->isFreeFragment() )
			delete *current;
}

double MemoryFrame::getHeightInRows() const
{
	return rowCount + memoryRoofRows;
}

bool MemoryFrame::allocate(MemoryAllocation* memoryAllocation)
{
	// Find free space to allocate the variable
	MemoryAllocations::iterator freeFragment = memoryAllocations.end();
	VisAddress offset = -1;
	if ( findSmallestFreeFragmentToAllocate(memoryAllocation, freeFragment, offset) == false )
	{
		// There is not space to allocate the variable in visualization, animate segment overflow
		qCDebug(logTemporary) << "Memory Frame: no space to allocate" << memoryAllocation->name << "of" << memoryAllocation->size << "bytes";
		return false;
	}

	// We found empty space, the iterator points to it
	// If there is a positive offet, empty space will left in front of the variable
	if ( offset > 0 )
		memoryAllocations.insert( freeFragment, new MemoryAllocation(AllocationSegment::free, offset, (*freeFragment)->visualizationAddress ) );

	// Allocate the variable in front of the free fragment
	memoryAllocations.insert( freeFragment, memoryAllocation );
	memoryAllocation->visualizationAddress = (*freeFragment)->visualizationAddress + offset;

	// Extract the memory from the free fragment
	(*freeFragment)->reduceSize(offset + memoryAllocation->size);

	printAllocationQueue();

	// The variable was allocated
	// ToDo: add variable to scene and update interface
	return true;
}

bool MemoryFrame::findSmallestFreeFragmentToAllocate(const MemoryAllocation* variable, MemoryAllocations::iterator& smallestFragment, VisAddress& offset)
{
	// Not found values
	smallestFragment = memoryAllocations.end();
	offset = -1;

	// Search for free memory
	for ( MemoryAllocations::iterator current = memoryAllocations.begin(); current != memoryAllocations.end(); ++current )
	{
		// If this is free space, get the offset where the variable would be allocated
		VisAddress currentOffset = (*current)->calculateAllocationOffset(variable, startByte);

		// Negative offset means that itr memory is unable to allocate the variable
		if ( currentOffset < 0 )
			continue;

		// If current is the first free fragment we found, or current is smaller than the one we
		// known free at the moment, take current as a better option
		if ( smallestFragment == memoryAllocations.end() || (*current)->size < (*smallestFragment)->size )
		{
			smallestFragment = current;
			offset = currentOffset;
		}
	}

	return smallestFragment != memoryAllocations.end();
}

void MemoryFrame::printAllocationQueue()
{
	// Print allocations
	QString output("Allocation queue ");
	for ( MemoryAllocations::iterator current = memoryAllocations.begin(); current != memoryAllocations.end(); ++current )
		output += QString("[%1|%2|%3]").arg( (*current)->visualizationAddress ).arg( (*current)->size ).arg( (*current)->name );
	qCCritical(logTemporary).noquote() << output;
}

bool MemoryFrame::deallocate(MemoryAllocation* memoryAllocation)
{
	// ToDo: implement deallocation
	qCCritical(logTemporary) << "Memory Frame: Deallocating" << memoryAllocation->name << "releasing" << memoryAllocation->size << "bytes";
	return true;
}

void MemoryFrame::buildMemoryFrame(qreal zValue)
{
	// Create the memory roof
	Q_ASSERT(scene);
	Q_ASSERT(memoryTop == nullptr);
	memoryTop = new MemoryTop(rowSize, scene, zValue);
	addItem(memoryTop, memoryRoofRows / getHeightInRows(), zValue);

	// Create the memory rows
	size_t rowStartByte = startByte;
	for (size_t index = 0; index < rowCount; ++index)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, scene, zValue, memoryAllocations.first()->hasGarbage);
		addItem(memoryRow, 1.0 / getHeightInRows(), zValue);
		memoryRows.append( memoryRow );
		rowStartByte += rowSize;
	}
}
