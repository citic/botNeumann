#include "LayoutItem.h"

LayoutItem::LayoutItem(qreal proportion)
	: proportion(proportion)
	, margins{0}
	, maintainAspectRatio(false)
	, floating(false)
{
}

LayoutItem::~LayoutItem()
{
}

void LayoutItem::setMargins(qreal sameForAll)
{
	for ( int i = marginTop; i < marginCount; ++i )
		margins[i] = sameForAll;
}

void LayoutItem::setMargins(qreal topBottom, qreal leftRight)
{
	margins[marginTop] = margins[marginBottom] = topBottom;
	margins[marginLeft] = margins[marginRight] = leftRight;
}

void LayoutItem::setMargins(qreal top, qreal leftRight, qreal bottom)
{
	margins[marginTop] = top;
	margins[marginLeft] = margins[marginRight] = leftRight;
	margins[marginBottom] = bottom;
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

	qreal marginTopHeight = margins[marginTop] * height;
	qreal marginRightWidth = margins[marginRight] * width;
	qreal marginBottomHeight = margins[marginBottom] * height;
	qreal marginLeftWidth = margins[marginLeft] * width;

	left += marginLeftWidth;
	top += marginTopHeight;
	width -= marginLeftWidth + marginRightWidth;
	height -= marginTopHeight + marginBottomHeight;
}
