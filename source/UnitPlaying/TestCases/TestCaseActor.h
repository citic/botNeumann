#ifndef TESTCASEACTOR_H
#define TESTCASEACTOR_H

#include "Actor.h"

class TestCaseActor : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(TestCaseActor)

  protected:
	/// The number of this test case in playerSolution
	int index = -1;

  public:
	/// Constructor
	explicit TestCaseActor(int index, QGraphicsItem* parentItem);
};

#endif // TESTCASEACTOR_H
