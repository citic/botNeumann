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

  protected:
	/// When characters float, they do over the scene instead of stdin/stdout buffers
	Scene* scene = nullptr;
	/// This chacter was read or written by this thread
	ExecutionThread* executionThread = nullptr;
	/// The index of this character moving to/from the thread
	int index = 0;
	/// The amount of other characters traveling to/from the thread
	int length = 0;

  public:
	/// Constructor
	GraphicCharValue(QGraphicsItem* graphicsParent, qreal zValue, const QString& value);
	/// Starts the several-state animation from the current place to the given execution thread
	/// @return The duration of the animation in milliseconds
	int animateRead(int index, int length, int ioBufferCapacity, ExecutionThread* targetThread, Scene* scene);
	/// Place this character within the thread position
	/// This method is called when the character is written by a thread. Its initial position is
	/// within the thread according to its index and the length of the write operation
	void placeInThread(int index, int length, int ioBufferCapacity, ExecutionThread* thread, Scene* scene);

  protected slots:
	/// Animates this character leaving the stdin tube and move to the execution thread stored
	/// in the class member pointer
	int animateMoveToThread();
	/// Called when read or write animation is finished
	void removeCharFromScene();
	/// Make this character float over the scene. After this method is called, the character
	/// remains at the same position, but it can be freely moved through the scene
	/// ToDo: we use the scene, but it may eventually changed for any other QGraphicsItem object
	bool reparentTo(Scene* newParent);

  protected:
	/// Calculates the horizontal percent of this character in its thread
	qreal calculateHorizontalScenePercent() const;
	/// Calculates the vertical percent of this character in its thread
	qreal calculateVerticalScenePercent() const;
};

#endif // GRAPHICCHARVALUE_H
