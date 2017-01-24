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

void LabelButton::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Update the LayoutItem part of this object
	LayoutItem::resize(left, top, width, height);
	applyMargins(left, top, width, height);

	// Resize the QGraphicaItem part of this object
	resizeItem(this, left, top, width, height);
}
