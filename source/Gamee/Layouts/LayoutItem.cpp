#include "LayoutItem.h"

LayoutItem::LayoutItem(qreal proportion)
	: mainProportion(proportion)
{
}

LayoutItem::~LayoutItem()
{
}

void LayoutItem::resize(qreal left, qreal top, qreal width, qreal height)
{
	this->layoutLeft = left;
	this->layoutTop = top;
	this->layoutWidth = width;
	this->layoutHeight = height;
}

void LayoutItem::updateLayoutItem()
{
	resize(layoutLeft, layoutTop, layoutWidth, layoutHeight);
}

void LayoutItem::setMargins(qreal top, qreal right, qreal bottom, qreal left)
{
	margins[marginTop] = top;
	margins[marginRight] = right;
	margins[marginBottom] = bottom;
	margins[marginLeft] = left;
}

void LayoutItem::applyMargins(qreal& left, qreal& top, qreal& width, qreal& height)
{
	// ToDo: avoid reducing margins when there are not enough space

	// Calculate each margin in pixes and store the results because they are used twice below
	qreal marginTopHeight = getPixelsForMargin(marginTop, height);
	qreal marginRightWidth = getPixelsForMargin(marginRight, width);
	qreal marginBottomHeight = getPixelsForMargin(marginBottom, height);
	qreal marginLeftWidth = getPixelsForMargin(marginLeft, width);

	// Reduce the available space according to the number of pixels for each margin
	left += marginLeftWidth;
	top += marginTopHeight;
	width -= marginLeftWidth + marginRightWidth;
	height -= marginTopHeight + marginBottomHeight;
}

qreal LayoutItem::getPixelsForMargin(int margin, qreal edge) const
{
	return margins[margin] > -1.0 && margins[margin] < 1.0 ? margins[margin] * edge : margins[margin];
}
