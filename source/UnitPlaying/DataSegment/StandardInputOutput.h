#ifndef STANDARDINPUTOUTPUT_H
#define STANDARDINPUTOUTPUT_H

#include "MemorySegment.h"
#include "AlignedItem.h"

#include <QGraphicsRectItem>

class GraphicValue;
class Prop;

class InputOutputBuffer : public QGraphicsRectItem, public LayoutItem, public AlignedItem
{
	Q_DISABLE_COPY(InputOutputBuffer)

  protected:
	/// zValue where the characteres will be placed on the scene
	qreal zValue = 0.0;

  public:
	/// Constructor
	explicit InputOutputBuffer(qreal zValue, QGraphicsItem* parent);
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
	/// Sets the Z-index provided by layouts to the QGraphicsItem system
	virtual void setZ(qreal zValue) override { setZValue(zValue); }
};

/// Base class that represents a standard input, output or error object with a tube
/// They behave as a memory segment
class StandardInputOutput : public MemorySegment
{
	Q_DISABLE_COPY(StandardInputOutput)

  protected:
	/// Tester placed in standard output
	Prop* tester = nullptr;
	/// An area to show the characters moving through the tube
	InputOutputBuffer* buffer = nullptr;
	/// Characteres traveling by the tube
	QList<GraphicValue*> characters;

  public:
	/// @param type One of the following: "input", "output", "error"
	explicit StandardInputOutput(const QString& type, Unit& unit, Scene* scene);

  protected:
	/// Load graphic elements to represent this object
	/// @param type The type of standard input or output this graphical object represents. It must
	/// be one of the following: "input", "output", "error"
	void buildStandardInputOutput(QString type);
	/// Builds the are where characteres will travel inside the tube
	void buildBuffer(const QString& type, Scene* scene);
};


/// Manages the standard input getting data from the current test case input file
class StandardInput : public StandardInputOutput
{
	Q_DISABLE_COPY(StandardInput)

  public:
	/// Constructor
	explicit StandardInput(Unit& unit, Scene* scene);
	/// Loads the active test case input file, and animates characters arriving by the input tube
	bool loadFile(const QString& inputFilepath);
};


/// Manages the standard ouput writing data
class StandardOutput : public StandardInputOutput
{
	Q_DISABLE_COPY(StandardOutput)

  public:
	/// Constructor
	explicit StandardOutput(Unit& unit, Scene* scene);
};

#endif // STANDARDINPUTOUTPUT_H
