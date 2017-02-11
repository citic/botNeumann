#include "TestCaseActor.h"

TestCaseActor::TestCaseActor(int index, QGraphicsItem* parentItem)
	: Actor("up_standard_output_test_inactive1", parentItem)
	, index(index)
{
	Q_ASSERT(index >= 1);
}
