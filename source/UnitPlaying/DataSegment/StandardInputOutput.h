#ifndef STANDARDINPUTOUTPUT_H
#define STANDARDINPUTOUTPUT_H

#include "MemorySegment.h"

/// Represents a standard input, output or error object with a tube
/// They behave as a memory segment
class StandardInputOutput : public MemorySegment
{
	Q_DISABLE_COPY(StandardInputOutput)

  public:
	/// The type of standard input or output this graphical object represents
	/// Must be one of the following: "input", "output", "error"
	QString type;

  public:
	/// @param type One of the following: "input", "output", "error"
	explicit StandardInputOutput(const QString& type, Unit& unit, Scene* scene);

  protected:
	/// Load graphic elements to represent this object
	void buildStandardInputOutput();
};

#endif // STANDARDINPUTOUTPUT_H
