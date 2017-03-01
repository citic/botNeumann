#ifndef MEMORYROW_H
#define MEMORYROW_H

#include "Common.h"
#include "LinearLayout.h"

struct MemoryAllocation;
class Scene;

/**
	@brief A row or line of memory represented as a shelf in botNeumann
 */
class MemoryRow : public LinearLayout
{
	Q_DISABLE_COPY(MemoryRow)

  public:
	/// Memory rows are represented as shelves. These shelves allocate variables (values). Shelves
	/// have a space to labels. We can display on them one of the following: memory addresses (the
	/// default), variable names, variable data types. User can switch the label type anytime s/he
	/// wants
	enum class LabelType
	{
		memoryAddresses,
		variableNames,
		dataTypes
	};

  protected:
	/// This row starts at this byte, it could be also called offset
	VisAddress start;
	/// This is the size of the memory row
	VisAddress size;
	/// To reparent children to this scene
	Scene* scene;
	/// The type of lables to show under each shelf
	LabelType labelType;
	/// The z-index where this memory row is shown
	qreal zValue = 0.0;

  public:
	/// Constructor
	explicit MemoryRow(VisAddress start, VisAddress size, Scene* scene, qreal zValue, bool withGarbage);
	/// Destructor
	~MemoryRow();
	/// Display memory addresses on labels
	void displayMemoryAddresses();
	/// Display variable names on labels
	void displayVariableNames();
	/// Display data types on labels
	void displayDataTypes();
	/// Cycle between label types
	void cycleLabelType();
	/// Allocate the given variable in this memory row if the variable is within the range of this
	/// memory row.
	/// @return true if the variable is entirelly allocated, false if the variable is not
	/// allocated at all or there is pending bytes of the variable to be allocated in the next
	/// memory row
	bool allocate(MemoryAllocation* variable);

  protected:
	/// Build the shelves and labels
	void buildMemoryRow(bool withGarbage);
	/// Build labels for memory addresses
	void buildMemoryAddresses();
	/// Fills this memory row with garbage in each byte
	void fillWithGarbage();
	/// Calculates the horizontal proportion of a byte in this memory row
	inline qreal getByteProportion() const { return 1.0 / (size + 2.0); }
	/// Calculate the intersection between the addresses of the variable and the addresses of this
	/// memory row.@return true if there is intersection, false if intersection is empty
	bool calculateIntersection(const MemoryAllocation* variable, VisAddress& firstByte, VisAddress& lastByte);
};

#endif // MEMORYROW_H
