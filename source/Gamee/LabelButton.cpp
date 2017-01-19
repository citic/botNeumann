#include "LabelButton.h"
#include "BotNeumannApp.h"

#include <QBrush>
#include <QFont>

LabelButton::LabelButton(const QString& label, QGraphicsItem* parentItem)
	: QGraphicsSimpleTextItem(label, parentItem)
{
	setFont(QFont(BotNeumannApp::getRobotFontName()));
	setBrush(QBrush(Qt::cyan));
	setFlags( flags() | QGraphicsItem::ItemIsSelectable );
}

LabelButton::~LabelButton()
{
}

void LabelButton::setText(const QString& text)
{
	QGraphicsSimpleTextItem::setText(text);
	updateLayoutItem();
}

void LabelButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	emit pressed();
	QGraphicsSimpleTextItem::mouseReleaseEvent(event);
}
