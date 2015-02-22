#include "BotNeumannApp.h"
#include "LabelButton.h"
#include <QBrush>
#include <QFont>

LabelButton::LabelButton(const QString& label, QGraphicsItem* parentItem)
	: ScenicElement(label, parentItem)
{
	setFont(QFont(BotNeumannApp::getRobotFontName()));
	setBrush(QBrush(Qt::cyan));
	setFlags( flags() | QGraphicsItem::ItemIsSelectable );
}

LabelButton::~LabelButton()
{
}

// ToDo: Remove code redundancy, see SvgButton::itemChange()
QVariant LabelButton::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
	// If the seletion of this element changed and now it is selected, alert other objects
	if (change == QGraphicsItem::ItemSelectedChange && value.toBool() )
		emit pressed();

	return ScenicElement::itemChange(change, value);
}
