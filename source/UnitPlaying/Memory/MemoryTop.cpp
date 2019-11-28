#include "MemoryTop.h"
#include "Actor.h"
#include "BotNeumannApp.h"
#include "MultiSvgButton.h"

#include <QBrush>
#include <QFont>

MemoryTop::MemoryTop(size_t size, const QString& labelText, QGraphicsItem* graphicsParentItem, qreal zValue)
	: LinearLayout(Qt::Horizontal)
	, size(size)
	, graphicsParentItem(graphicsParentItem)
	, zValue(zValue)
{
	buildMemoryTop();
	buildLabel(labelText);
}

void MemoryTop::buildMemoryTop()
{
	Q_ASSERT(graphicsParentItem);

  #if ABSTRACT
	addItem( new Actor("up_rectangle", graphicsParentItem), 1.0, zValue );
  #else
	// The left and right extremes of the shelf require 1 byte each
	const double leftRightProportion = 1.0 / (size + 2.0);
	// The middle element occupies the rest
	const double middleProportion = size / (size + 2.0);

	// Create the images
	Actor* leftRoof = new Actor("up_memory_top_left", graphicsParentItem);
	Actor* middleRoof = new Actor("up_memory_top_middle", graphicsParentItem);
	Actor* rightRoof = new Actor("up_memory_top_right", graphicsParentItem);

	#if ABSTRACT
		leftRoof->setPaddings  (-0.01, -0.195, -0.05, 0.0);
		middleRoof->setPaddings(0.0, -0.005, -0.005, 0.0);
		rightRoof->setPaddings (-0.01, 0.0, -0.05, -0.195);
	#endif

	// Add them to the layout
	addItem(leftRoof, leftRightProportion, zValue);
	addItem(middleRoof, middleProportion, zValue);
	addItem(rightRoof, leftRightProportion, zValue);
  #endif
}

void MemoryTop::buildLabel(const QString& labelText)
{
	// Create a label that uses an array of images (represented by a piece of tape)
	Q_ASSERT(label == nullptr);
	QStringList labelAssets;
  #if ABSTRACT
	//labelAssets << "up_variable_name_middle";
  #else
	labelAssets << "up_variable_name_left" << "up_variable_name_middle" << "up_variable_name_right";
  #endif

	// Calculate proportions of each part of the tape, using the bytes they require
	QList<qreal> proportions;

  #if ABSTRACT
	const qreal labelBytes = qMin( labelText.length() / 1.66, qreal(size) );
  #else
	const qreal labelBytes = qMin( labelText.length() / 2.85, qreal(size) );
	const qreal leftRightProportion = 3.246 / 11.951 / labelBytes;
	const qreal middleProportion =  1.0 - 2 * leftRightProportion;
	proportions << leftRightProportion << middleProportion << leftRightProportion;
  #endif

	// Create the label and add it to the scene
	label = new MultiSvgButton(labelAssets, proportions, graphicsParentItem, labelText, zValue + 0.01 );
	label->setFont(QFont(BotNeumannApp::getMonospacedFontName()));
	label->setBrush(QBrush(Qt::black));
	label->setShear(-0.2, 0.0);

  #if ABSTRACT
	insertItem(label, 1.25 / size, labelBytes /size, zValue + 0.01);
  #else
	const qreal refTopHeight = 21.737;
	label->setPaddingTop( 1.128 / refTopHeight );
	label->setPaddingBottom( 5.756 / refTopHeight );
	insertItem(label, 1.25 / (size + 2.0), labelBytes / (size + 2.0), zValue + 0.01);
  #endif
}
