#ifndef STANDARDINPUTOUTPUT_H
#define STANDARDINPUTOUTPUT_H

#include "LinearLayout.h"

class Scene;

/// Represents a standard input, output or error object with a tube
class StandardInputOutput : public LinearLayout
{
	Q_DISABLE_COPY(StandardInputOutput)

  public:
	/// The type of standard input or output this graphical object represents
	/// Must be one of the following: "input", "output", "error"
	QString type;
	/// The scene where this graphical object is drawn. Required to add the props that compose
	/// the tubes
	Scene* scene;

  public:
	/// @param type One of the following: "input", "output", "error"
	explicit StandardInputOutput(const QString& type, Scene* scene);

  protected:
	/// Load graphic elements to represent this object
	void buildStandardInputOutput();
};

#endif // STANDARDINPUTOUTPUT_H
