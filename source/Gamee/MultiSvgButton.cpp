#include "MultiSvgButton.h"
#include "BotNeumannApp.h"
#include "LabelButton.h"
#include "Prop.h"
#include "MultiSvgButton.h"

#include <QBrush>
#include <QFont>


MultiSvgButton::MultiSvgButton(const QStringList& prefixedSvgElementIds, const QList<qreal>& proportions, QGraphicsItem* parentItem, const QString& text, qreal zValue)
	: LinearLayout(Qt::Horizontal)
{
	Q_ASSERT( prefixedSvgElementIds.count() == proportions.count() );
	background.reserve( prefixedSvgElementIds.count() );
	for ( int index = 0; index < prefixedSvgElementIds.count(); ++index )
	{
		Prop* back = new Prop(prefixedSvgElementIds[index], parentItem);
		addItem(back, proportions[index], zValue);
		background.append(back);
	}

	buildLabel(text, parentItem, zValue);
}

MultiSvgButton::~MultiSvgButton()
{
}

void MultiSvgButton::setFont(const QFont& font)
{
	if ( label )
		label->setFont(font);
}

void MultiSvgButton::setBrush(const QBrush& brush)
{
	if ( label )
		label->setBrush(brush);
}

void MultiSvgButton::setShear(qreal shearX, qreal shearY)
{
	foreach ( Prop* back, background )
		back->setShearX(shearX, shearY);

	if ( label )
		label->setShearX(shearX, shearY);
}

void MultiSvgButton::buildLabel(const QString& text, QGraphicsItem* parentItem, qreal zValue)
{
	if ( ! text.isEmpty() )
	{
		this->label = new LabelButton(text, parentItem);
		this->label->setMargins(0.1);
		addItem( this->label, 1.0, zValue + 0.01 );
	}
}

void MultiSvgButton::setEnabled(bool enabled)
{
	qreal transparency = enabled ? 1.0 : defaultDisabledOpacity;
	if ( label )
		label->setOpacity(transparency);
}
