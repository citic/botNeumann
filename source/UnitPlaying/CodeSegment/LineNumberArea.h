#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

class CodeEditor;

/**
	@brief Shows line numbers in the left margin of the code editor

	According to the Qt Code Editor Example, "we could simply paint the line numbers directly on the
	code editor, and drop the LineNumberArea class. However, the QWidget class helps us to scroll()
	its contents. Also, having a separate widget is the right choice if we wish to extend the editor
	with breakpoints or other code editor features. The widget would then help in the handling of
	mouse events."
**/
class LineNumberArea : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(LineNumberArea)

  protected:
	/// This class is exclusive for this code editor
	CodeEditor* codeEditor;

  public:
	/// Constructor
	explicit LineNumberArea(CodeEditor* codeEditor);
	/// Destructor
	~LineNumberArea();
	/// ?
	virtual QSize sizeHint() const override;

  protected:
	/// Called when this widet must be painted
	virtual void paintEvent(QPaintEvent* event) override;
	/// Toggles breakpoints if user presses a line number
	virtual void mousePressEvent(QMouseEvent *event) override;
};

#endif // LINENUMBERAREA_H
