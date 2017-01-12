#ifndef EXECUTIONTHREADACTOR_H
#define EXECUTIONTHREADACTOR_H

#include "LinearLayout.h"

class Actor;
class LabelButton;
class Scene;

class ExecutionThreadActor : public LinearLayout
{
	Q_DISABLE_COPY(ExecutionThreadActor)

  protected:
	/// Actor number, used to retrieve assets: front, back, side
	int actorNumber = 1;
	/// To reparent children
	Scene* scene = nullptr;
	/// Actor image
	Actor* actor = nullptr;
	/// Shows the line number being executed by the execution thread
	LabelButton* lineNumber = nullptr;

  public:
	/// Constructor
	/// @param id Number of process, given by the debugger
	/// @param scene To reparent children to the scene
	explicit ExecutionThreadActor(int id, Scene* scene);
	/// Given the sequential number (id) of a thread, it assigns a number of actor (robot)
	/// @remarks Assume there are 4 actors (robots) in assets
	static inline int mapActorNumber(int id) { return (id - 1) % 4 + 1; }
	/// Animate the apparition of the robot
	/// @return The duration of the animation in milliseconds
	int animateAppear();
	/// Animate the vanishing of the robot
	/// @return The duration of the animation in milliseconds
	int animateDisappear();
	/// Changes the line number within the actor's display
	/// @return The duration of the animation in milliseconds
	int updateLineNumber(int updatedLineNumber);

  protected:
	/// Build the label to show line numbers
	void buildActor();
};

#endif // EXECUTIONTHREADACTOR_H
