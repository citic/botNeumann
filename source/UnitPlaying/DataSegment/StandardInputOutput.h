#ifndef STANDARDINPUTOUTPUT_H
#define STANDARDINPUTOUTPUT_H

#include "Actor.h"
#include "MemorySegment.h"
#include "RectLayoutItem.h"

class GraphicCharValue;
class ExecutionThread;
class OutputTester;

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
	/// Simulates a file cursor: the byte where the next read or write operation will take place
	/// Notice that @a textPosition indicates the amount of bytes ready to be read in stdin, and
	/// they are visibe in the tube, while that @a cursor indicates the next byte to be read by
	/// an execution thread
	int cursor = 0;

  public:
	/// Constructor
	explicit InputOutputBuffer(Scene* scene, qreal zValue, int capacity);
	/// Set the text to be read (standard input) or printed (standard output)
	inline void setText(const QString& text) { this->text = text; }
	/// Get access to the capacity of this buffer
	inline int getCapacity() const { return capacity; }
	/// Get a copy of the cursor
	inline int getCursor() const { return cursor; }
	/// Animate buffering (filling) the standard input. It fills all empty spaces with pending
	/// characters in @a text
	/// @param charsToFill Send the required number of characters to be loaded in the buffer,
	/// -1 if only the available empty space in the tube should be filled
	/// @return The duration of the animation in milliseconds
	int animateFill();
	/// Animate the read of @a length characters, which are extracted from standard input tube
	/// The remaining empty space is filled calling @a animateFill() function
	/// @return The duration of the animation in milliseconds
	int animateRead(int length, const QList<ExecutionThread*>& waitingQueue);
	/// Animate writing @a length characters from the execution thread to the output buffer.
	/// The output tester will be turned green if player solution's output matches the test case's
	/// expected output, or red otherwise.
	/// @return The duration of the animation in milliseconds
	int animateWrite(int length, const QList<ExecutionThread*>& waitingQueue, OutputTester* tester);
	/// Returns the amount of free space or characters
	inline int getFreeCharacters() const { return capacity - characters.count(); }
	/// Clear all the values on the buffer
	/// @return The duration in milliseconds of the animation
	int clear();
};

class OutputTester : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(OutputTester)

  protected:
	/// The test case's expected output
	QString text;
	/// Indicates the next character to be written. Simulates a cursor
	int cursor = 0;
	/// True if the player solution output is valid until now
	bool valid = true;

  public:
	/// Constructor
	/// @see Actor::Actor()
	explicit OutputTester(QGraphicsItem* parentItem);
	/// Set the text of the test case's expected output
	inline void setText(const QString& text) { this->text = text; cursor = 0; }
	/// Test a character passing behind the tester. If character does not match the next expected
	/// character, the tester becomes red. If character is the first one, the tester becomes green.
	/// Otherwise it keeps its current state
	/// @return The duration of the animation in milliseconds
	int test(const QString& character);
	/// Clear the test. It becomes inactive
	void clear();
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
	OutputTester* tester = nullptr;
	/// An area to show the characters moving through the tube
	InputOutputBuffer* buffer = nullptr;

  public:
	/// @param type The type of standard input or output this graphical object represents. It must
	/// be one of the following: "input", "output", "error"
	explicit StandardInputOutput(Type type, Unit& unit, Scene* scene);
	/// Loads the active test case input file, and animates characters arriving by the input tube
	bool loadInputFile(const QString& filepath);
	/// Loads a pair of output files. The player solution's output file is compared character by
	/// character against to the test case's expected output file. If they math, the tester becomes
	/// green. If they do not, the tester becomes red and stays red.
	bool loadOutputFiles(const QString& playerSolutionOutput, const QString& expectedOutput);
	/// Animate buffering (filling) the standard input. It fills all empty spaces with pending
	/// characters in @a text
	/// @return The duration of the animation in milliseconds
	inline int animateFill() { return buffer->animateFill(); }
	/// Removes all values from the buffer
	int clear();
	/// Updates the cursor and animates bytes leaving to or entering in the tube
	/// @return Number of bytes read or written, 0 if no cursor was changed
	int updateCursor(int cursor, const QList<ExecutionThread*>& waitingQueue, int& maxDuration);

  protected:
	/// Load graphic elements to represent this object
	void buildStandardInputOutput();
	/// Builds the are where characteres will travel inside the tube
	void buildBuffer(size_t bufferSize, Scene* scene);
	/// Load the given file as text of the buffer. It does not animate input buffer fill
	/// @return true on success
	bool loadFile(const QString& filepath, const QString& target = "buffer");
};

#endif // STANDARDINPUTOUTPUT_H
