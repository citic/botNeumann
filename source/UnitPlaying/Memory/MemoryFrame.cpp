#include "MemoryFrame.h"
#include "LogManager.h"
#include "MemoryAllocation.h"
#include "MemoryRow.h"
#include "MemoryTop.h"

// The roof requires half memory row
const double memoryRoofRows = 0.5;

MemoryFrame::MemoryFrame(QGraphicsItem* graphicsParentItem, size_t rowCount, size_t startByte, size_t rowSize, const QString& topLabel, qreal zValue, bool withGarbage)
	: LinearLayout(Qt::Vertical)
	, graphicsParentItem(graphicsParentItem)
	, rowCount(rowCount)
	, startByte(startByte)
	, rowSize(rowSize)
{
	// Initially there is just one big fragment of free memory
	MemoryAllocation* freeFragment = new MemoryAllocation(AllocationSegment::free, getSize(), startByte );
	freeFragment->hasGarbage = withGarbage;
	memoryAllocations.append(freeFragment);

	// Draw the memory frame in the visualization
	buildMemoryFrame(topLabel, zValue);

	// Distribute the free fragment to all the memory rows. This is required in order to draw
	// initial garbage if this segment has it by default
	printAllocationQueue();
	distributeVariablesIntoMemoryRows();
}

MemoryFrame::~MemoryFrame()
{
	removeMemoryAllocations();
}

void MemoryFrame::removeMemoryAllocations()
{
	// Free fragments are not added to the scene because they are invisible
	for ( MemoryAllocations::iterator current = memoryAllocations.begin(); current != memoryAllocations.end(); ++current )
		if ( (*current)->isFreeFragment() )
			delete *current;

	memoryAllocations.clear();
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

	// We look for the smallest free fragment able to allocate the given variable
	// If the function finds such fragment, it will update our freeFragment iterator and our offset
	if ( findSmallestFreeFragmentToAllocate(memoryAllocation, freeFragment, offset) == false )
	{
		// There is not space to allocate the variable in visualization, animate segment overflow
		qCDebug(logTemporary) << "Memory Frame: no space to allocate" << memoryAllocation->name << "of" << memoryAllocation->size << "bytes";
		return false;
	}

	// We found empty space, the iterator points to it
	// If there is a positive offet, empty space (padding) will remain in front of the variable
	if ( offset > 0 )
		memoryAllocations.insert( freeFragment, new MemoryAllocation(AllocationSegment::free, offset, (*freeFragment)->visualizationAddress ) );

	// Allocate the variable in first bytes of the free fragment, after the offset (if any)
	memoryAllocations.insert( freeFragment, memoryAllocation );
	memoryAllocation->visualizationAddress = (*freeFragment)->visualizationAddress + offset;

	// Extract the assigned memory from the free fragment
	(*freeFragment)->reduceSize(offset + memoryAllocation->size);

	printAllocationQueue();

	// The variable must be placed in one or more memory rows, distribute it
	return distributeVariablesIntoMemoryRows();
}

bool MemoryFrame::findSmallestFreeFragmentToAllocate(const MemoryAllocation* variable, MemoryAllocations::iterator& smallestFragment, VisAddress& offset)
{
	// Not found values
	smallestFragment = memoryAllocations.end();
	offset = -1;

	// Search for free memory
	for ( MemoryAllocations::iterator fragmentIterator = memoryAllocations.begin(); fragmentIterator != memoryAllocations.end(); ++fragmentIterator )
	{
		// If this is free space, get the offset where the variable would be allocated
		VisAddress currentOffset = (*fragmentIterator)->calculateAllocationOffset(variable, startByte);

		// Negative offset means that itr memory is unable to allocate the variable
		if ( currentOffset < 0 )
			continue;

		// Check if we have found a smaller (minimum) free fragment
		if ( smallestFragment == memoryAllocations.end() || (*fragmentIterator)->size < (*smallestFragment)->size )
		{
			// current is the first free fragment we found, or current is smaller than the one we
			// known at the moment. Save current as our better known option
			smallestFragment = fragmentIterator;
			offset = currentOffset;
		}
	}

	return smallestFragment != memoryAllocations.end();
}

bool MemoryFrame::distributeVariablesIntoMemoryRows()
{
	// We iterate for all variables assigned to this frame, distributing them to its memory rows
	MemoryAllocations::iterator variableIterator = memoryAllocations.begin();
	int currentRow = 0;

	// Traverse all the variables
	while ( variableIterator != memoryAllocations.end() )
	{
		// For convenience
		MemoryAllocation* variable = *variableIterator;

		// We must allocate a variable, if all rows were filled, we have a segment overflow
		if ( currentRow >= memoryRows.count() )
			return false;

		// If the current row did not accept the variable, or accepted it partially
		if ( memoryRows[currentRow]->allocate(variable) == false )
		{
			// Try to allocate the variable in the next row
			++currentRow;
			continue;
		}

		// The row accepted the variable, and it is completely allocated, move to the next variable
		++variableIterator;
	}

	// All variables were distributed in the available rows
	return true;
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

bool MemoryFrame::deallocateAll()
{
	// Visible variables are allocated in memory rows, not in the frame directly
	for ( int index = 0; index < memoryRows.count(); ++index )
		memoryRows[index]->deallocateAll();

	// Remove invisible memory allocations
	removeMemoryAllocations();
	return true;
}

void MemoryFrame::buildMemoryFrame(const QString& topLabel, qreal zValue)
{
	// Create the memory roof
	Q_ASSERT(graphicsParentItem);
	Q_ASSERT(memoryTop == nullptr);
	memoryTop = new MemoryTop(rowSize, topLabel, graphicsParentItem, zValue);
	addItem(memoryTop, memoryRoofRows / getHeightInRows(), zValue);

	// Create the memory rows
	size_t rowStartByte = startByte;
	for (size_t index = 0; index < rowCount; ++index)
	{
		MemoryRow* memoryRow = new MemoryRow(rowStartByte, rowSize, graphicsParentItem, zValue, memoryAllocations.first()->hasGarbage);
		addItem(memoryRow, 1.0 / getHeightInRows(), zValue);
		memoryRows.append( memoryRow );
		rowStartByte += rowSize;
	}
}
