#ifndef EXECUTIONTHREADACTOR_H
#define EXECUTIONTHREADACTOR_H

#include "Actor.h"

class LabelButton;
class Scene;

class QColor;

class ExecutionThreadActor : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(ExecutionThreadActor)

  protected:
	/// Actor number, used to retrieve assets: front, back, side
	int actorNumber = 1;
	/// Shows the line number being executed by the execution thread
	LabelButton* lineNumber = nullptr;

  public:
	/// Constructor
	/// @param id Number of process, given by the debugger
	/// @param scene To reparent children to the scene
	explicit ExecutionThreadActor(int threadId, QGraphicsItem* parentItem);
	/// Given the sequential number (id) of a thread, it assigns a number of actor (robot)
	/// @remarks Assume there are 4 actors (robots) in assets
	static inline int mapActorNumber(int id);
	/// Changes the line number within the actor's display
	/// @return The duration of the animation in milliseconds
	int updateLineNumber(int updatedLineNumber);
	/// Returns the color of this thread, in order to highlight its running line
	const QColor& getHighlightColor() const;
	/// Animates the robot turning. At the end of the animation the robot will be facing the camera
	/// @return the duration in milliseconds of the animation
	int turnFront();
	/// Animates the robot turning. At the end of the animation the robot's back will be visible
	/// @return the duration in milliseconds of the animation
	int turnBack();

  protected:
	/// Build the label to show line numbers
	void buildActor();

  protected slots:
	/// Makes the line number visible
	void setLineNumberVisible();
};

#endif // EXECUTIONTHREADACTOR_H
