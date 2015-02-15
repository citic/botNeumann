#include "BotNeumannApp.h"
#include "SvgButton.h"
#include <QBrush>
#include <QFont>
#include <QGraphicsSimpleTextItem>

SvgButton::SvgButton(const QString& label, const QString& normalForeground, const QString& normalBackground, QGraphicsItem* parentItem)
	: ScenicElement( ! normalForeground.isEmpty() ? normalForeground : normalBackground, parentItem)
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

SvgButton* SvgButton::createLabelButton(const QString& label, const QString& normalBackground, QGraphicsItem* parentItem)
{
	return new SvgButton(label, "", normalBackground, parentItem);
}

SvgButton* SvgButton::createImageButton(const QString& normalForeground, QGraphicsItem* parentItem)
{
	return new SvgButton("", normalForeground, "", parentItem);
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


QVariant SvgButton::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
	if (change == QGraphicsItem::ItemSelectedChange)
	{
		bool newState = value.toBool();
		if ( newState )
			emit pressed();
	}
	return ScenicElement::itemChange(change, value);
}
