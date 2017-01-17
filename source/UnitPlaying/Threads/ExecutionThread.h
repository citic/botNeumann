#ifndef EXECUTIONTHREAD_H
#define EXECUTIONTHREAD_H

#include "LinearLayout.h"

class ExecutionThreadActor;
class GdbTreeNode;
class Scene;

class QColor;

/** An ExecutionThread is a graphical object to represent an execution thread running on the
	user program (inferior). An ExecutionThread that is running is shown in one of the available
	CpuCores. A sleeping ExecutionThread is not shown in any CpuCore.

	Execution threads are represented with two parts. 1: A robot shows the line number of the source
	code that the execution thread is running. 2: One or more stack frames represent the function
	calls that the execution thread is running. The top-most frame is the running function call.
**/
class ExecutionThread : public LinearLayout
{
	Q_DISABLE_COPY(ExecutionThread)

  protected:
	/// To reparent children to this scene
	Scene* scene;
	/// The number of execution thread, reported by Gdb
	int id = -1;
	/// The source file that generated the code that this thread is executing
	QString filename;
	/// True if this thread is idle, i.e: there are not enough cores to run this thread
	bool idle = false;
	/// The line number in that file being executed
	int lineNumber = -1;
	/// The function being currently executed by this thread
	QString functionName;
	/// The previous source file that this thread was executing before the last update
	QString previousFilename;
	/// The previous line number that this thread was executing before the last update
	/// It is used by code editors to clear highlighted lines
	int previousLineNumber = -1;
	/// The robot used to represent the execution thread
	ExecutionThreadActor* robot = nullptr;

  public:
	/// Constructor
	explicit ExecutionThread(Scene* scene, int id);
	/// Animate the apparition of the robot
	/// @return The duration of the animation in milliseconds
	int animateAppear();
	/// Animate the vanishing of the robot
	/// @return The duration of the animation in milliseconds
	int animateDisappear();
	/// Updates this execution thread from Gdb information. If execution thread is shown on the
	/// screen, the visual update is done immediately and the maxDuration may be set
	/// @return true if there was change
	bool updateFromDebugger(const GdbTreeNode* threadNode, int& maxDuration);
	/// Get access to the members
	inline int getId() const { return id; }
	inline const QString& getFilename() const { return filename; }
	inline bool isIdle() const { return idle; }
	void setIdle(bool idle);
	inline int getLineNumber() const { return lineNumber; }
	inline const QString& getPreviousFilename() const { return previousFilename; }
	inline int getPreviousLineNumber() const { return previousLineNumber; }
	/// Returns the color of this thread, in order to highlight its running line
	const QColor& getHighlightColor() const;

  protected:
	/// Build the robot
	void buildExecutionThread();
	/// The type of return of @a updateFilename()
	enum FilenameUpdateResult
	{
		fileIsTheSame,
		newFileInPlayerSolution,
		fileNotInPlayerSolution,
	};
	/// If the filename has changed, asks the CodeEditor to show it
	/// @return true if there was change
	FilenameUpdateResult updateFilename(const QString& updatedFilename, int& maxDuration);
	/// If the line number has changed, updates the number on the robot and asks the CodeEditor
	/// to highlight that line using the robot color
	/// @return true if there was change
	bool updateLineNumber(int updatedLineNumber, int& maxDuration);
	/// If the function being executed has changed, animates a function call
	/// @return true if there was change
	bool updateFunctionName(const QString& updatedFunctionName, int& maxDuration);
};

#endif // EXECUTIONTHREAD_H
