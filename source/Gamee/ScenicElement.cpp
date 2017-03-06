#include "ScenicElement.h"
#include "Layout.h"
#include "SvgRendererManager.h"

#include <QGraphicsScene>

ScenicElement::ScenicElement(const QString& prefixedSvgElementId, QGraphicsItem* parentItem)
	: QGraphicsSvgItem(parentItem)
{
	if ( ! prefixedSvgElementId.isEmpty() )
	{
		setRenderer( mapSceneIdFromPrefix(prefixedSvgElementId) );
		setElementId(prefixedSvgElementId);
	}
}

ScenicElement::ScenicElement(const QString& svgFilename, QGraphicsItem* parentItem, bool loadFile)
	: QGraphicsSvgItem(svgFilename, parentItem)
{
	Q_UNUSED(loadFile);
}

ScenicElement::~ScenicElement()
{
	delete layout;
}

void ScenicElement::removeFromScene()
{
	scene()->removeItem(this);
	this->deleteLater();
}

void ScenicElement::setRenderer(SceneId sceneId)
{
	setSharedRenderer( SvgRendererManager::findRenderer(sceneId) );
}

SceneId ScenicElement::mapSceneIdFromPrefix(const QString& prefixedText)
{
	if ( prefixedText.startsWith("up_") ) return sceneUnitPlaying;
	if ( prefixedText.startsWith("ge_") ) return sceneGeneral;
	if ( prefixedText.startsWith("gm_") ) return sceneGameMenu;
	if ( prefixedText.startsWith("us_") ) return sceneUnitSelection;

	return sceneGeneral;
}

void ScenicElement::resize(qreal left, qreal top, qreal width, qreal height)
{
	// Resize children before changing the values (left, top, width, height)
	if ( layout ) layout->resize(left, top, width, height);

	// Update the LayoutItem part of this object
	LayoutItem::resize(left, top, width, height);
	applyMargins(left, top, width, height);

	// Resize the QGraphicaItem part of this object
	resizeItem(this, left, top, width, height);
}
