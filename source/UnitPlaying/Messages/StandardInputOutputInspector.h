#ifndef STANDARDINPUTOUTPUTINSPECTOR_H
#define STANDARDINPUTOUTPUTINSPECTOR_H

#include <QSplitter>

class PlayerSolution;

class QAction;
class QTextEdit;
class QToolBar;

class StandardInputOutputInspector : public QSplitter
{
	Q_OBJECT
	Q_DISABLE_COPY(StandardInputOutputInspector)

  protected:
	/// Shows test case's input text (black) and consumed input by player solution (grey)
	QTextEdit* standardInputInspector = nullptr;
	/// A toolbar in middle of the text editor with options
	QToolBar* toolBar = nullptr;
	/// Shows expected test case's standard output (grey) and player solution output. The latter
	/// is (black) when matches the former, or (red) otherwise
	QTextEdit* standardOutputInspector = nullptr;
	/// Compares the player solution error output against test case's expected standard error
	QTextEdit* standardErrorInspector = nullptr;
	/// Toggles between standard output and standard error
	QAction* toggleOutputErrorAction = nullptr;
	/// We are visualizing input/output for this test case number
	int testCaseNumber = -1;

  public:
	/// Constructor
	explicit StandardInputOutputInspector(QWidget* parent = nullptr);
	/// Loads the expected input/output/error in the StandardInputOutputInspector from the test
	/// case files, and set the cursors to the first character of them
	bool loadTestCase(int testCaseNumber, PlayerSolution* playerSolution);

  public slots:

  protected:
	/// Builds the inspector with three areas
	void buildInspector();

  protected slots:
	/// Toggles between the standard output and standard error
	void toggleOutputError();
	/// Loads the content of a file into a text editor.
	/// @return true on success, false otherwise
	bool loadFileInto(const QString& filepath, QTextEdit* inspector);
};

#endif // STANDARDINPUTOUTPUTINSPECTOR_H
