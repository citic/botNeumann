#include "CodeEditorDockWidget.h"
#include "SyntaxHighlighter.h"
#include <QTextEdit>

// A tab is visualized as 3 space characters because screen size is reduced in the game
const int tabStop = 3;

CodeEditorDockWidget::CodeEditorDockWidget(QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(tr("Program"), parent, flags)
{
	setObjectName("codeEditor");
	setupToolbar();
	setupCodeEditor();
}

CodeEditorDockWidget::~CodeEditorDockWidget()
{
}

void CodeEditorDockWidget::setCode(const QString& code)
{
	codeEditor->setPlainText(code);
}

void CodeEditorDockWidget::setupToolbar()
{

}

void CodeEditorDockWidget::setupCodeEditor()
{
	// The code editor is a QTextEdit object
	codeEditor = new QTextEdit(this);

	// Set the default monospaced font of the operating system
//	QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	QFont font("Liberation Mono");
	font.setPointSize(11);
	font.setStyleHint(QFont::TypeWriter);
	font.setFixedPitch(true);
	codeEditor->setFont(font);

	// Make tabs the same size than 4 spaces
	QFontMetrics metrics(font);
	codeEditor->setTabStopWidth(tabStop * metrics.width(' '));

	// Create the object that will provide color to C++ code within the editor
	highlighter = new SyntaxHighlighter(codeEditor->document());

	// Place the code editor as the central widget of this dock widget
	setWidget(codeEditor);
}
