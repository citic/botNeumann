#include "BotNeumannApp.h"
#include "SvgButton.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsSimpleTextItem>


SvgButton::SvgButton(const QString& prefixedSvgElementId, QGraphicsItem* parentItem, const QString& text)
	: Actor(prefixedSvgElementId, parentItem)
{
	buildLabel(text);
	setFlag( QGraphicsItem::ItemIsSelectable );
}

SvgButton::~SvgButton()
{
}

void SvgButton::buildLabel(const QString& text)
{
	if ( ! text.isEmpty() )
	{
		// ToDo: replace parentItem by this
		this->label = new QGraphicsSimpleTextItem(text, this);
		this->label->setFont(QFont(BotNeumannApp::getRobotFontName()));
		this->label->setBrush(QBrush(Qt::cyan));

		qreal width = this->boundingRect().width();
		qreal height = this->boundingRect().height();
		label->setPos(0.2 * width, 0.23 * height);
		qreal sw = width * 0.6 / label->boundingRect().width();
		qreal sh = height * 0.6 / label->boundingRect().height();
		label->setTransform(QTransform().scale(sw, sh));
	}
	setPaddings(0.1); // I am not sure if it is a good padding default for buttons
}

void SvgButton::setEnabled(bool enabled)
{
	qreal transparency = enabled ? 1.0 : defaultDisabledOpacity;
	setOpacity(transparency);
	if ( label ) label->setOpacity(transparency);
	QGraphicsItem::setEnabled(enabled);
}

void SvgButton::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	emit pressed();
	QGraphicsItem::mouseReleaseEvent(event);
}
