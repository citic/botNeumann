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
	/// Pointer to the scene to place the chars (they float by the scene)
	Scene* scene = nullptr;
	/// zValue where the characteres will be placed on the scene
	qreal zValue = 0.0;
	/// Capacity in bytes of the buffer
	int capacity = 0;
	/// Characteres traveling by the tube
	QList<GraphicValue*> characters;
	/// The text of the test case or the generated output
	QString text;
	/// Cursor that indicates the next byte to be read or printed in the @a text string
	int cursor = 0;
	/// ToDo: inherit Linearlayout
	LinearLayout* characterLayout = nullptr;

  public:
	/// Constructor
	explicit InputOutputBuffer(Scene* scene, qreal zValue, int capacity);
	/// Resize this element
	/// This method is called each time the Stage and Scene has been resized
	virtual void resize(qreal left, qreal top, qreal width, qreal height) override;
	/// Sets the Z-index provided by layouts to the QGraphicsItem system
	virtual void setZ(qreal zValue) override { setZValue(zValue); }
	/// Set the text to be read (standard input) or printed (standard output)
	inline void setText(const QString& text) { this->text = text; cursor = 0; }
	/// Animate buffering (filling) the standard input. It fills all empty spaces with pending
	/// characters in @a text
	/// @return The duration of the animation in milliseconds
	int animateFill();
	/// Animate the read of @a length characters, which are extracted from standard input tube
	/// The remaining empty space is filled calling @a animateFill() function
	/// @return The duration of the animation in milliseconds
	int animateRead(int length);
	/// Returns the amount of free space or characters
	inline int getFreeCharacters() const { return capacity - characters.count(); }
	/// Returns the amount of available chars that are still not loaded in the tube
	inline int getPendingCharacters() const { return text.length() - cursor; }
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

  public:
	/// @param type One of the following: "input", "output", "error"
	explicit StandardInputOutput(const QString& type, Unit& unit, Scene* scene);
	/// Loads the active test case input file, and animates characters arriving by the input tube
	bool loadFile(const QString& inputFilepath);
	/// Animate buffering (filling) the standard input. It fills all empty spaces with pending
	/// characters in @a text
	/// @return The duration of the animation in milliseconds
	inline int animateFill() { return buffer->animateFill(); }
	/// Animate the read of @a length characters, which are extracted from standard input tube
	/// The remaining empty space is filled calling @a animateFill() function
	/// @return The duration of the animation in milliseconds
	inline int animateRead(int length) { return buffer->animateRead(length); }

  protected:
	/// Load graphic elements to represent this object
	/// @param type The type of standard input or output this graphical object represents. It must
	/// be one of the following: "input", "output", "error"
	void buildStandardInputOutput(QString type);
	/// Builds the are where characteres will travel inside the tube
	void buildBuffer(const QString& type, size_t bufferSize, Scene* scene);
};

#endif // STANDARDINPUTOUTPUT_H
