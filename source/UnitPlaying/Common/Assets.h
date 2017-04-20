#ifndef ASSETS_H
#define ASSETS_H


// Threads ---------------------------------------------------------------------------------------

#include <QColor>

/// The line number must appear within the display of the robot, but each robot has the display
/// at different top
const qreal lineNumberTopPadding[] = {30, 60, 58, 34};
const qreal lineNumberRefWidth =  33; // px
const qreal lineNumberRefHeight = 19; // px

const int alpha = 127;

/// Each actor (robot) has its own color
const QColor threadColors[] =
{
	QColor(  0,  90, 170, alpha), // thread 1 #005ab3 blue
	QColor(255, 192,   0, alpha), // thread 2 #ffc000 yellow
	QColor(222, 107,   0, alpha), // thread 3 #de6b00 orange
	QColor(114, 140,   0, alpha), // thread 4 #728c00 green
};


// Memory row ------------------------------------------------------------------------------------

// Garbage graphics are smaller than the memory row's height
const qreal refRowHeight = 34.879;

// Garbage graphics have different height, but we place all of them aligned to the bottom
// So, we need to calculate the top percent. These numbers come from the SVG dimensions
const qreal garbagePaddingTop[] =
{
	1.0 - ( 14.500 + 15.515 ) / refRowHeight,
	1.0 - ( 14.500 + 11.480 ) / refRowHeight,
	1.0 - ( 14.500 +  7.478 ) / refRowHeight,
};


// MemoryAllocation (variables) ------------------------------------------------------------------

// Position and dimensions of the variable label for name and value
const qreal variableLabelY = 15.067;
const qreal variableLabelHeight = 11.946;

// Convert Inkscape positions and dimensions to percents
enum refDataElements
{
	refPaddingTop,
	refPaddingBottom,
	refLabelTop,
	refLabelBottom,

	refElementsSize
};

#define RefPaddingTop(y, height)    (1.0 - (y + height) / refRowHeight)
#define RefPaddingBottom(y)         (y / refRowHeight)
#define RefLabelTop(y, height)     (((y + height) - (variableLabelY + variableLabelHeight)) / height)
#define RefLabelBottom(y, height)  ((variableLabelY - y) / height)

#define RefPaddingArray(type, y, height) \
	const qreal ref ## type ## Paddings[] =\
	{ \
		RefPaddingTop(y, height) ,\
		RefPaddingBottom(y) ,\
		RefLabelTop(y, height) ,\
		RefLabelBottom(y, height) \
	}; \
	//static_assert( sizeof(ref ## type ## Paddings) / sizeof(qreal) == refElementsSize );


RefPaddingArray(Bool, 11.529, 16.500)
RefPaddingArray(Char, 11.529, 16.681)
RefPaddingArray(Int, 12.545, 15.500)
RefPaddingArray(Float, 12.545, 15.500)
RefPaddingArray(Pointer, 12.545, 21.944)
RefPaddingArray(Struct, 12.545, 19.500)


// StandardInputOutput ----------------------------------------------------------------------------

const qreal refTubeWidth = 384.644;
const qreal refTubeHeight = 38.800;
const qreal refBufferTop = 35.762;
const qreal refBufferBottom = 15.108;
const qreal refBufferLeft = 29.653;
const qreal refBufferRight = 353.631;

const qreal refCouplingWidth = 4.970;
const qreal refCouplingHeight = 25.776;
const qreal refOpeningWidth = 162.979;
const qreal refOpeningHeight = 38.799;

#endif // ASSETS_H
