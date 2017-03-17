#ifndef STANDARDINPUTOUTPUT_H
#define STANDARDINPUTOUTPUT_H

#include "MemorySegment.h"

class GraphicValue;

/// Base class that represents a standard input, output or error object with a tube
/// They behave as a memory segment
class StandardInputOutput : public MemorySegment
{
	Q_DISABLE_COPY(StandardInputOutput)

  protected:
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
