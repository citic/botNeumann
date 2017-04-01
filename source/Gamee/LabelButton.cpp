#include "LabelButton.h"
#include "Actor.h"
#include "BotNeumannApp.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsScene>

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

void LabelButton::removeFromScene()
{
	scene()->removeItem(this);
	this->deleteLater();
}

void LabelButton::setText(const QString& text, bool updateLayout)
{
	QGraphicsSimpleTextItem::setText(text);
	if ( updateLayout )
		updateLayoutItem();
}

int LabelButton::animateAppear(int duration, int initialDelay, qreal fromOpacity, qreal toOpacity)
{
	return Actor::animateAppear(this, duration, initialDelay, fromOpacity, toOpacity);
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
