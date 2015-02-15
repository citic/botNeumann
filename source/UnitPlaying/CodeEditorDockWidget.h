#ifndef CODEEDITORDOCKWIDGET_H
#define CODEEDITORDOCKWIDGET_H

#include <QDockWidget>

class QTextEdit;
class SyntaxHighlighter;

class CodeEditorDockWidget : public QDockWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(CodeEditorDockWidget)

  protected:
	/// Object where the actual source code is shown and edited
	QTextEdit* codeEditor;
	/// Store formatting rules for C++
	SyntaxHighlighter* highlighter;

  public:
	/// Constructor
	explicit CodeEditorDockWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	/// Destructor
	virtual ~CodeEditorDockWidget();
	/// Get access to the code editor
	inline QTextEdit* getCodeEditor() const { return codeEditor; }
	/// Set the give code in the editor
	void setCode(const QString& code);

  protected:
	/// Create and set up the tool bar with undo, copy, run, debug and other actions
	void setupToolbar();
	/// Create and configure the text editor object
	void setupCodeEditor();
};

#endif // CODEEDITORDOCKWIDGET_H
