#include "MemoryFrame.h"
#include "Actor.h"
#include "LogManager.h"
#include "MemoryAllocation.h"
#include "MemoryRow.h"
#include "MemoryTop.h"

// The roof requires half memory row
const double memoryRoofRows = 0.5;
const double memoryLegsRows = 1.0 / 3.0;

MemoryFrame::MemoryFrame(QGraphicsItem* graphicsParentItem, size_t rowCount, size_t startByte, size_t rowSize, const QString& topLabel, qreal zValue, bool withGarbage, bool withLegs, size_t maxSize)
	: LinearLayoutActor(Qt::Vertical)
	, graphicsParentItem(graphicsParentItem)
	, zValue(zValue)
	, rowCount(rowCount)
	, startByte(startByte)
	, rowSize(rowSize)
	, maxSize(maxSize)
	, withGarbage(withGarbage)
	, withLegs(withLegs)
{
	// Initially there is just one big fragment of free memory. We select the size for the free
	// stack space among the real size between the actual size and the allowed max size
	size_t freeSize = qMax( getSize(), maxSize );
	MemoryAllocation* freeFragment = new MemoryAllocation( AllocationSegment::free, freeSize, startByte );
	freeFragment->hasGarbage = withGarbage;
	memoryAllocations.append(freeFragment);

	// Draw the memory frame in the visualization
	buildMemoryFrame(topLabel);

	// Distribute the free fragment to all the memory rows. This is required in order to draw
	// initial garbage if this segment has it by default
	printAllocationQueue();
	distributeVariablesIntoMemoryRows(0);
}

MemoryFrame::~MemoryFrame()
{
	removeMemoryAllocations();
}

void MemoryFrame::buildMemoryFrame(const QString& topLabel)
{
	// Create the memory roof
	Q_ASSERT(graphicsParentItem);
	Q_ASSERT(memoryTop == nullptr);
	memoryTop = new MemoryTop(rowSize, topLabel, graphicsParentItem, zValue);
	addItem(memoryTop, memoryRoofRows / getHeightInRows(), zValue);

	// Create the initial memory rows
	createRows(rowCount, startByte);

	// If asked, create memory legs
	if ( withLegs )
		buildMemoryLegs(zValue);
}

bool MemoryFrame::createRows(size_t rowCount, size_t fromByte)
{
	// Create each memory row
	for ( size_t index = 0; index < rowCount; ++index )
	{
		MemoryRow* memoryRow = new MemoryRow(fromByte, rowSize, graphicsParentItem, zValue, withGarbage);
		addItem(memoryRow, 1.0 / getHeightInRows(), zValue);
		memoryRows.append( memoryRow );
		fromByte += rowSize;
	}

	return true;
}

void MemoryFrame::buildMemoryLegs(qreal zValue)
{
	// We need a layout to place the legs vertically
	Q_ASSERT(legsLayout == nullptr);
	legsLayout = new LinearLayout(Qt::Horizontal);
	Actor* leftLegs = new Actor("up_memory_row_base_left1", graphicsParentItem);
	Actor* rightLegs = new Actor("up_memory_row_base_right1", graphicsParentItem);

	// We use the same byte proportion than any memory row
	qreal byteProportion = 1.0 / (rowSize + 2.0);

	// We place the legs in the edge positions
	legsLayout->insertItem( leftLegs, 0.0, 1.0 * byteProportion, zValue );
	legsLayout->insertItem( rightLegs, (rowSize + 1) * byteProportion, 1.0 * byteProportion, zValue );

	// We add the horizontal layout to this object
	addItem( legsLayout, memoryLegsRows / getHeightInRows(), zValue );
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
	return rowCount + memoryRoofRows + withLegs * memoryLegsRows;
}

int MemoryFrame::allocate(MemoryAllocation* memoryAllocation, int initialDelay)
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
		return -1;
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
	return distributeVariablesIntoMemoryRows(initialDelay);
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

int MemoryFrame::distributeVariablesIntoMemoryRows(int initialDelay)
{
	// The duration of this animation
	int duration = 0;

	// We iterate for all variables assigned to this frame, distributing them to its memory rows
	MemoryAllocations::iterator variableIterator = memoryAllocations.begin();
	int currentRow = 0;

	// Traverse all the variables
	while ( variableIterator != memoryAllocations.end() )
	{
		// For convenience
		MemoryAllocation* variable = *variableIterator;

		// We must allocate a variable, if all rows were filled
		if ( currentRow >= memoryRows.count() )
		{
			// We do not need to grow the frame for free memory
			if ( variable->isFreeFragment() )
			{
				// Skip to next variable
				++variableIterator;
				continue;
			}

			// All the memory rows are filled, but if the memory frame can grow, create more rows
			// ToDo: Only a piece of the variable may be needed to allocate, not the entire variable
			int requiredRows = (qMax(variable->size, 1ll) + (rowSize - 1)) / rowSize;
			qCCritical(logTemporary(), "Growing %d rows for %s of %lld bytes", requiredRows, qPrintable(variable->name), variable->size);

			// If it is unable to grow more, we have a segment overflow
			int growDuration = grow(requiredRows, initialDelay);
			if ( growDuration < 0 )
				return growDuration;

			// Add this duration to the proccess and the remaining animations
			duration += growDuration;
			initialDelay += growDuration;

			// Be sure there is a memory row to continue
			Q_ASSERT( currentRow < memoryRows.count() );
		}

		// Try to allocate this variable in the current row
		int entirelyAllocated = false;
		int rowAllocation = memoryRows[currentRow]->allocate(variable, entirelyAllocated, initialDelay);

		// Update the durations of animation
		duration += rowAllocation;
		initialDelay += rowAllocation;

		// If the variable was partially allocated or not allocated at all
		if ( entirelyAllocated <= 0 )
		{
			// Try to allocate the variable in the next row
			++currentRow;
			continue;
		}

		// The row accepted the variable, and it is completely allocated, move to the next variable
		++variableIterator;
	}

	// All variables were distributed in the available rows
	return duration;
}

int MemoryFrame::grow(int extraRows, int initialDelay)
{
	// If there is enough memory
	Q_ASSERT(extraRows > 0);
	if ( getSize() + extraRows * rowSize > maxSize )
		return -1;

	// The new memory rows will start after the last memory row or the beginning there isn't any
	size_t fromByte = memoryRows.count() > 0 ? memoryRows[memoryRows.count() - 1]->getLastByte() : startByte;

	// There is enough memory, create the rows
	if ( ! createRows(extraRows, fromByte) )
		return false;

	// The number of rows grew
	rowCount = memoryRows.count();

	// We have added more rows, we have to update the proportions of all rows
	updateRowProportions();

	// ToDo: Animate the new rows raising up from the memory interface of the cpu core
	Q_UNUSED(initialDelay);
	return 0;
}

// Utility function to update the start and proportion of one row, and returns the proportion
qreal updateRowProportion(LayoutItem* item, qreal start, qreal proportion)
{
	item->setMainStartProportion(start);
	item->setMainProportion(proportion);
	return proportion;
}

void MemoryFrame::updateRowProportions()
{
	// Each row is below the previous ones. We accumulate proportion in this variable
	qreal accProportion = 0.0;

	// Memory top
	accProportion += updateRowProportion( memoryTop, accProportion, memoryRoofRows / getHeightInRows() );

	// Each memory row
	for ( int index = 0; index < memoryRows.count(); ++index )
		accProportion += updateRowProportion( memoryRows[index], accProportion, 1.0 / getHeightInRows() );

	// Legs, if any
	if ( legsLayout )
		updateRowProportion(legsLayout, accProportion, memoryLegsRows / getHeightInRows() );
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
