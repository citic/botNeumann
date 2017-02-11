#ifndef TESTCASEACTOR_H
#define TESTCASEACTOR_H

#include "Actor.h"
#include <QProcess>

class PlayerSolution;

class TestCaseActor : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(TestCaseActor)

  protected:
	/// The number of this test case in playerSolution
	int index = -1;
	/// The player solution to be tested
	PlayerSolution* playerSolution = nullptr;
	/// Process to call the player solution executable
	QProcess* process = nullptr;

  protected:
	/// Test case arguments file path
	QString args;
	/// Test case input file path
	QString input;
	/// Test case expected output file path
	QString output_ex;
	/// Player solution generated output file path
	QString output_ps;
	/// Test case expected error output file path
	QString error_ex;
	/// Player solution generated error output file path
	QString error_ps;

  public:
	/// Constructor
	explicit TestCaseActor(int index, QGraphicsItem* parentItem);
	/// Run player solution executable against this test case
	/// This is an asynchronic process. This method only starts it
	bool testPlayerSolution(PlayerSolution* playerSolution);

  protected slots:
	/// Called when player solution has finished running agianst this test case
	void playerSolutionFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // TESTCASEACTOR_H
