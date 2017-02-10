#ifndef TESTCASEMANAGER_H
#define TESTCASEMANAGER_H

#include "LinearLayout.h"
#include <QObject>

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

  public:
	/// Constructor
	explicit TestCaseManager(Scene* scene, QObject* parent = nullptr);

  protected:
	/// Build a region to place future test cases (an empty tube)
	void buildTestCaseManager();
};

#endif // TESTCASEMANAGER_H
