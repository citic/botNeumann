#ifndef TESTCASEACTOR_H
#define TESTCASEACTOR_H

#include "Actor.h"
#include <QProcess>

class PlayerSolution;

class TestCaseActor : public Actor
{
	Q_OBJECT
	Q_DISABLE_COPY(TestCaseActor)

  public:
	/// Result of a test case after running
	enum TestCaseResult
	{
		unknown,
		passed,
		failed,
	};

  protected:
	/// The number of this test case in playerSolution
	int index = -1;
	/// The player solution to be tested
	PlayerSolution* playerSolution = nullptr;
	/// Process to call the player solution executable
	QProcess* process = nullptr;
	/// Result of this test case
	TestCaseResult result = unknown;
	/// Reason on case of failed
	QString failReason;
	/// True if this is the test case being visualized. The active test case does not show the
	/// result until the visualization is finished
	bool isTheActiveTestCase = false;

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
	/// The first character where player solution's output differs from unit's expected output
	/// -1 means there are not differences
	long firstDiffOutput = -2;
	/// The first character where player solution's error differs from unit's expected error
	/// -1 means there are not differences
	long firstDiffError = -2;

  public:
	/// Constructor
	explicit TestCaseActor(int index, QGraphicsItem* parentItem);
	/// Run player solution executable against this test case
	/// This is an asynchronic process. This method only starts it
	bool testPlayerSolution(PlayerSolution* playerSolution);
	/// Sets the result of the test case, and updates the color of the tester (green for passed,
	/// red for failed, gray for unknown)
	void setTestCaseResult(TestCaseResult result, const QString& reason = "");
	/// Set this test case as the active one (being visualized)
	/// @param updateInterface Send true if the actor should change its face to reflect the
	/// activation or de-activation change
	void setActiveTestCase(bool active, bool updateInterface = false);
	/// Returns true if this test case was passed by player solution
	inline bool isPassed() const { return result == passed; }
	/// Set the actor face according to the test case result
	/// @param hideActiveTestCaseResult Send true if the unknown result must be shown when this is
	/// the active test case being visualized. Send false, if the interface must update to the
	/// result anyway. The latter is the case when the active test case has finished the
	/// visualization and the test case actor must update
	void reflectTestCaseResult(bool hideActiveTestCaseResult);
	/// Updates the tool tip to reflect the result of the test case
	void updateToolTip(const QString& result);

  signals:
	/// Emitted when user presses this test case in order to make it active
	void testCaseActivated(int testCaseNumber);

  protected slots:
	/// Called when player solution has finished running agianst this test case
	void playerSolutionFinished(int exitCode, QProcess::ExitStatus exitStatus);
	/// Called when player solution took more time than the allowed (timeout)
	void playerSolutionTimeout();

  protected:
	/// Overriden to manage click or tap events
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // TESTCASEACTOR_H
