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

QRectF LabelButton::boundingRect() const
{
	return QRectF(0.0, 0.0, layoutWidth, layoutHeight);
}

void LabelButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	emit pressed();
	QGraphicsSimpleTextItem::mouseReleaseEvent(event);
}

void LabelButton::resize(qreal left, qreal top, qreal width, qreal height)
{
	prepareGeometryChange();
	setPos(left, top);
	LayoutItem::resize(left, top, width, height);
}
