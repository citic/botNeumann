#include "Spacer.h"

Spacer::Spacer()
{
}

Spacer::~Spacer()
{
}

void Spacer::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Space does not require anything
	Q_UNUSED(left);
	Q_UNUSED(top);
	Q_UNUSED(width);
	Q_UNUSED(height);
}
