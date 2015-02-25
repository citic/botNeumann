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

void LabelButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	emit pressed();
	QGraphicsItem::mouseReleaseEvent(event);
}
