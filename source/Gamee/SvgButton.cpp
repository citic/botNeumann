#include "BotNeumannApp.h"
#include "SvgButton.h"
#include <QBrush>
#include <QFont>
#include <QGraphicsSimpleTextItem>

SvgButton::SvgButton(const QString& imageFileName, QGraphicsItem* parentItem, const QString& label)
	: ScenicElement(imageFileName, parentItem)
	, label(nullptr)
{
	if ( ! label.isEmpty() )
	{
		this->label = new QGraphicsSimpleTextItem(label, parentItem);
		this->label->setFont(QFont(BotNeumannApp::getRobotFontName()));
		this->label->setBrush(QBrush(Qt::cyan));
	}
	setMargins(0.1); // I am not sure if it is a good margin default for buttons
	setFlags( flags() | QGraphicsItem::ItemIsSelectable );
}

SvgButton::~SvgButton()
{
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
	qreal transparency = enabled ? 1.0 : 0.15;
	setOpacity(transparency);
	if ( label ) label->setOpacity(transparency);
	QGraphicsItem::setEnabled(enabled);
}

QVariant SvgButton::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
	// If the seletion of this element changed and now it is selected, alert other objects
	if (change == QGraphicsItem::ItemSelectedChange && value.toBool() )
		emit pressed();

	return ScenicElement::itemChange(change, value);
}
