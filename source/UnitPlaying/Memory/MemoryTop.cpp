#include "MemoryTop.h"
#include "BotNeumannApp.h"
#include "MultiSvgButton.h"
#include "Prop.h"

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

	// The left and right extremes of the shelf require 1 byte each
	const double leftRightProportion = 1.0 / (size + 2.0);
	// The middle element occupies the rest
	const double middleProportion = size / (size + 2.0);

	// Create the images
	Prop* leftRoof = new Prop("up_memory_top_left", graphicsParentItem);
	Prop* middleRoof = new Prop("up_memory_top_middle", graphicsParentItem);
	Prop* rightRoof = new Prop("up_memory_top_right", graphicsParentItem);

	// Add them to the layout
	addItem(leftRoof, leftRightProportion, zValue);
	addItem(middleRoof, middleProportion, zValue);
	addItem(rightRoof, leftRightProportion, zValue);
}

void MemoryTop::buildLabel(const QString& labelText)
{
	// Create a label that uses an array of images (represented by a piece of tape)
	Q_ASSERT(label == nullptr);
	QStringList labelAssets;
	labelAssets << "up_variable_name_left" << "up_variable_name_middle" << "up_variable_name_right";

	// Calculate proportions of each part of the tape, using the bytes they require
	const qreal labelBytes = qMin( labelText.length() / 2.85, qreal(size) );
	const qreal leftRightProportion = 3.246 / 11.951 / labelBytes;
	const qreal middleProportion =  1.0 - 2 * leftRightProportion;
	QList<qreal> proportions;
	proportions << leftRightProportion << middleProportion << leftRightProportion;

	// Create the label and add it to the scene
	label = new MultiSvgButton(labelAssets, proportions, graphicsParentItem, labelText, zValue + 0.01 );
	const qreal refTopHeight = 21.737;
	label->setMarginTop( 1.128 / refTopHeight );
	label->setMarginBottom( 5.756 / refTopHeight );
	label->setFont(QFont(BotNeumannApp::getMonospacedFontName()));
	label->setBrush(QBrush(Qt::black));
	label->setShear(-0.2, 0.0);
	insertItem(label, 1.25 / (size + 2.0), labelBytes / (size + 2.0), zValue + 0.01);
}
