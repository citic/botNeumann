#include "BotNeumannApp.h"
#include "SvgButton.h"
#include <QBrush>
#include <QFont>
#include <QGraphicsSimpleTextItem>

SvgButton::SvgButton(SceneId sceneId, const QString& svgElementId, QGraphicsItem* parentItem  , const QString& text)
	: ScenicElement(sceneId, svgElementId, parentItem)
{
	buildLabel(text);
}

SvgButton::SvgButton(const QString& prefixedSvgElementId, QGraphicsItem* parentItem, const QString& text)
	: ScenicElement(prefixedSvgElementId, parentItem)
{
	buildLabel(text);
}

SvgButton::~SvgButton()
{
}

void SvgButton::buildLabel(const QString& text)
{
	if ( ! text.isEmpty() )
	{
		// ToDo: replace parentItem by this
		this->label = new QGraphicsSimpleTextItem(text, QGraphicsSvgItem::parentItem());
		this->label->setFont(QFont(BotNeumannApp::getRobotFontName()));
		this->label->setBrush(QBrush(Qt::cyan));
	}
	setMargins(0.1); // I am not sure if it is a good margin default for buttons
	setFlags( flags() | QGraphicsItem::ItemIsSelectable );
}

void SvgButton::resize(qreal left, qreal top, qreal width, qreal height)
{
	ScenicElement::resize(left, top, width, height);
	if ( label )
	{
		label->setPos(left + 0.2 * width, top + 0.23 * height);
		qreal sw = width * 0.6 / label->boundingRect().width();
		qreal sh = height * 0.6 / label->boundingRect().height();
		label->setTransform(QTransform().scale(sw, sh));
	}
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
