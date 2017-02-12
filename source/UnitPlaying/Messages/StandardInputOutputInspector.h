#ifndef STANDARDINPUTOUTPUTINSPECTOR_H
#define STANDARDINPUTOUTPUTINSPECTOR_H

#include <QSplitter>

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

  public:
	/// Constructor
	explicit StandardInputOutputInspector(QWidget* parent = nullptr);

  public slots:

  protected:
	/// Builds the inspector with three areas
	void buildInspector();

  protected slots:
	/// Toggles between the standard output and standard error
	void toggleOutputError();
};

#endif // STANDARDINPUTOUTPUTINSPECTOR_H
