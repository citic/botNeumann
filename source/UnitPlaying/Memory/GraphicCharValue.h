#ifndef GRAPHICCHARVALUE_H
#define GRAPHICCHARVALUE_H

#include "GraphicValue.h"

class ExecutionThread;
class Scene;

/** A GraphicCharValue is a GraphicValue that can travel by any standard input/output tube,
	exit from or enter to it when it arrives to the open tube's area, travel by the scene towars/
	from any execution thread, and convert to other value.

	This complex animation is implemented by this class using a state machine.
*/
class GraphicCharValue : public GraphicValue
{
	Q_OBJECT
	Q_DISABLE_COPY(GraphicCharValue)

  public:
	/// States of the read/write animation
	enum AnimationState
	{
		animIdle,
		animInputBuffer,
		animLeavingBuffer,
		animToThread,
		animToBuffer,
		animEnteringBuffer,
		animOutputBuffer,
	};

  protected:
	/// Current state of the animation of this character
	AnimationState currentState = animIdle;
	/// The final target state of the animation
	AnimationState finalState = animIdle;

  public:
	/// Constructor
	GraphicCharValue(QGraphicsItem* graphicsParent, qreal zValue, const QString& value);
	/// Set the current state of the animation
	inline void setCurrentAnimationState(AnimationState state) { this->currentState = state; }
	/// Set the final state of the animation
	inline void setFinalAnimationState(AnimationState state) { this->finalState = state; }
	/// Starts the several-state animation from the current place to the given execution thread
	/// @return The duration of the animation in milliseconds
	int animateRead(int index, int length, int ioBufferCapacity, ExecutionThread* targetThread, Scene* scene);
};

#endif // GRAPHICCHARVALUE_H
