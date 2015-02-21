#include "LabelButton.h"

LabelButton::LabelButton(QGraphicsItem* parentItem)
	: ScenicElement(parentItem)
{
}

LabelButton::LabelButton(const QString& label, QGraphicsItem* parentItem)
	: ScenicElement(label, parentItem)
{
}

LabelButton::~LabelButton()
{
}
