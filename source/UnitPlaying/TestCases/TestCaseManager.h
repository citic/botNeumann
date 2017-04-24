#ifndef TESTCASEMANAGER_H
#define TESTCASEMANAGER_H

#include "LinearLayout.h"
#include <QObject>
#include <QVector>

class PlayerSolution;
class Scene;
class TestCaseActor;

/**
	Runs test cases using the player solution, and reports passed or failed test cases
	using visual metaphors (testers over a tube).

	This is both, a controller and view class.
*/
class TestCaseManager : public QObject, public LinearLayout
{
	Q_OBJECT
	Q_DISABLE_COPY(TestCaseManager)

  protected:
	/// To reparent children to this scene
	Scene* scene;
	/// To get the test cases and the player solution executable
	PlayerSolution* playerSolution = nullptr;
	/// The list of test cases actors
	QList<TestCaseActor*> testCaseActors;
	/// Manages the list of test cases of the current run
	LinearLayout* testCaseLayout = nullptr;
	/// Number of the active test case being visualized, by default 1
	int activeTestCase = 1;

  public:
	/// Constructor
	explicit TestCaseManager(Scene* scene, QObject* parent = nullptr);
	/// Set the player solution for later use
	inline void setPlayerSolution(PlayerSolution* playerSolution) { this->playerSolution = playerSolution; }
	/// Get the count of test cases
	inline int getTestCaseCount() const { return testCaseActors.count(); }
	/// Get the number of passed test cases by player solution
	/// @remarks Call this method after all test cases have been run
	int countPassedTestCases() const;

  signals:
	/// Emitted when user changed the test case being visualized
	void activeTestCaseChanged(int newTestCaseNumber);

  public slots:
	/// Called when all test cases and player solution has been built
	/// This method starts the process of running the player solution against the generated cases
	bool testPlayerSolution();
	/// Called when visualization has finished in order to remove old testers
	void clearAnimation();
	/// Called when user asked to change the test case being visualized
	void setActiveTestCase(int newTestCaseNumber);
	/// Called when player solution stopped normally and the active test case result must be
	/// reflected on this manager. This is needed because the active test case light stays "off"
	/// while the test case is being animated
	void playerSolutionFinished();

  protected:
	/// Build a region to place future test cases (an empty tube)
	void buildTestCaseManager();
	/// Creates a tester control for the test case which index is given. The tester control is
	/// inactive by default (gray). The test case @a index is gathered from the playerSolution.
	/// The player solution is run against that test case. If player solution produces the
	/// expected standard output and the standard error output, the tester will become green.
	/// Otherwise it becomes red.
	/// @param index The number of the test case in the player solution
	/// @param testerWidthPercent The proportion of just one test case control
	bool createAndRunTestCase( int index, const qreal testerWidthProportion );
};

#endif // TESTCASEMANAGER_H
