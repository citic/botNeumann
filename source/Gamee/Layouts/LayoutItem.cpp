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

void LayoutItem::setPaddings(qreal top, qreal right, qreal bottom, qreal left)
{
	paddings[paddingTop] = top;
	paddings[paddingRight] = right;
	paddings[paddingBottom] = bottom;
	paddings[paddingLeft] = left;
}

void LayoutItem::applyPaddings(qreal& left, qreal& top, qreal& width, qreal& height)
{
	// ToDo: avoid reducing paddings when there are not enough space

	// Calculate each paddings in pixes and store the results because they are used twice below
	qreal paddingTopHeight = getPixelsForPadding(paddingTop, height);
	qreal paddingRightWidth = getPixelsForPadding(paddingRight, width);
	qreal paddingBottomHeight = getPixelsForPadding(paddingBottom, height);
	qreal paddingLeftWidth = getPixelsForPadding(paddingLeft, width);

	// Reduce the available space according to the number of pixels for each padding
	left += paddingLeftWidth;
	top += paddingTopHeight;
	width -= paddingLeftWidth + paddingRightWidth;
	height -= paddingTopHeight + paddingBottomHeight;
}

qreal LayoutItem::getPixelsForPadding(int padding, qreal edge) const
{
	return paddings[padding] > -1.0 && paddings[padding] < 1.0 ? paddings[padding] * edge : paddings[padding];
}
