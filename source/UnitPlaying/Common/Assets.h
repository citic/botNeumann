#ifndef ASSETS_H
#define ASSETS_H

// Threads

#include <QColor>

/// The line number must appear within the display of the robot, but each robot has the display
/// at different top
const qreal lineNumberTopMargin[] = {30, 60, 58, 34};
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


// Memory row

// Garbage graphics are smaller than the memory row's height
const qreal refRowHeight = 34.879;

// Garbage graphics have different height, but we place all of them aligned to the bottom
// So, we need to calculate the top percent. These numbers come from the SVG dimensions
const qreal garbageMarginTop[] =
{
	1.0 - ( 14.500 + 15.515 ) / refRowHeight,
	1.0 - ( 14.500 + 11.480 ) / refRowHeight,
	1.0 - ( 14.500 +  7.478 ) / refRowHeight,
};

// Convert Inkscape positions and dimensions to percents
#define RefMarginTop(name, y, height) const qreal name = 1.0 - (y + height) / refRowHeight;
#define RefMarginBottom(name, y) const qreal name = y / refRowHeight;
#define RefMarginTopBottom(type, y, height) \
	RefMarginTop(type ## MarginTop, y, height) \
	RefMarginBottom(type ## MarginBottom, y)

// MemoryAllocation (variables)
RefMarginTopBottom(bool, 11.529, 16.500)
RefMarginTopBottom(char, 11.529, 16.681)
RefMarginTopBottom(int, 12.545, 15.500)
RefMarginTopBottom(float, 12.545, 15.500)
RefMarginTopBottom(pointer, 12.545, 21.944)
RefMarginTopBottom(struct, 12.545, 19.500)

#endif // ASSETS_H
