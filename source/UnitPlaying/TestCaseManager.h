#ifndef TESTCASEMANAGER_H
#define TESTCASEMANAGER_H

#include "LinearLayout.h"
#include <QObject>

class PlayerSolution;
class Scene;

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

  public:
	/// Constructor
	explicit TestCaseManager(Scene* scene, QObject* parent = nullptr);
	/// Set the player solution for later use
	inline void setPlayerSolution(PlayerSolution* playerSolution) { this->playerSolution = playerSolution; }

  public slots:
	/// Called when all test cases and player solution has been built
	/// This method starts the process of running the player solution against the generated cases
	bool testPlayerSolution();

  protected:
	/// Build a region to place future test cases (an empty tube)
	void buildTestCaseManager();
};

#endif // TESTCASEMANAGER_H
