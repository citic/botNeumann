#ifndef STANDARDINPUTOUTPUT_H
#define STANDARDINPUTOUTPUT_H

#include "MemorySegment.h"
#include "RectLayoutItem.h"

class GraphicCharValue;
class ExecutionThread;
class Actor;

class InputOutputBuffer : public RectLayoutItem
{
	Q_DISABLE_COPY(InputOutputBuffer)

  protected:
	/// Capacity in bytes of the buffer
	int capacity = 0;
	/// Characteres traveling by the tube
	QList<GraphicCharValue*> characters;
	/// The text of the test case or the generated output
	QString text;
	/// Index that indicates the next byte to be read or printed in the @a text string
	int fillCursor = 0;
	/// Simulates a file cursor: the byte where the next read or write operation will take place
	/// Notice that @a textPosition indicates the amount of bytes ready to be read in stdin, and
	/// they are visibe in the tube, while that @a cursor indicates the next byte to be read by
	/// an execution thread
	int cursor = 0;

  public:
	/// Constructor
	explicit InputOutputBuffer(Scene* scene, qreal zValue, int capacity);
	/// Set the text to be read (standard input) or printed (standard output)
	inline void setText(const QString& text) { this->text = text; fillCursor = 0; }
	/// Get a copy of the cursor
	inline int getCursor() const { return cursor; }
	/// Animate buffering (filling) the standard input. It fills all empty spaces with pending
	/// characters in @a text
	/// @param charsToFill Send the required number of characters to be loaded in the buffer,
	/// -1 if only the available empty space in the tube should be filled
	/// @return The duration of the animation in milliseconds
	int animateFill(int charsToFill = -1);
	/// Animate the read of @a length characters, which are extracted from standard input tube
	/// The remaining empty space is filled calling @a animateFill() function
	/// @return The duration of the animation in milliseconds
	int animateRead(int length, const QList<ExecutionThread*>& waitingQueue);
	/// Returns the amount of free space or characters
	inline int getFreeCharacters() const { return capacity - characters.count(); }
	/// Returns the amount of available chars that are still not loaded in the tube
	inline int getPendingCharacters() const { return text.length() - fillCursor; }
	/// Clear all the values on the buffer
	/// @return The duration in milliseconds of the animation
	int clear();
};

/// Base class that represents a standard input, output or error object with a tube
/// They behave as a memory segment
class StandardInputOutput : public MemorySegment
{
	Q_DISABLE_COPY(StandardInputOutput)

  public:
	enum Type
	{
		standardInput,
		standardOutput,
		standardError,

		standardIoUnknown,
	};

  protected:
	/// The type of this tube
	Type type = standardIoUnknown;
	/// Tester placed in standard output
	Actor* tester = nullptr;
	/// An area to show the characters moving through the tube
	InputOutputBuffer* buffer = nullptr;

  public:
	/// @param type The type of standard input or output this graphical object represents. It must
	/// be one of the following: "input", "output", "error"
	explicit StandardInputOutput(Type type, Unit& unit, Scene* scene);
	/// Loads the active test case input file, and animates characters arriving by the input tube
	bool loadFile(const QString& inputFilepath);
	/// Animate buffering (filling) the standard input. It fills all empty spaces with pending
	/// characters in @a text
	/// @return The duration of the animation in milliseconds
	inline int animateFill() { return buffer->animateFill(); }
	/// Animate the read of @a length characters, which are extracted from standard input tube
	/// The remaining empty space is filled calling @a animateFill() function
	/// @return The duration of the animation in milliseconds
	inline int animateRead(int length, const QList<ExecutionThread*>& waitingQueue) { return buffer->animateRead(length, waitingQueue); }
	/// Removes all values from the buffer
	inline int clear() { return buffer->clear(); }
	/// Updates the cursor and animates bytes leaving to or entering in the tube
	bool updateCursor(int cursor, const QList<ExecutionThread*>& waitingQueue, int& maxDuration);

  protected:
	/// Load graphic elements to represent this object
	void buildStandardInputOutput();
	/// Builds the are where characteres will travel inside the tube
	void buildBuffer(size_t bufferSize, Scene* scene);
};

#endif // STANDARDINPUTOUTPUT_H
