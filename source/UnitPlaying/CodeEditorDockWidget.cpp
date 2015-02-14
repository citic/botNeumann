#include "CodeEditorDockWidget.h"
#include <QTextEdit>

const int tabStop = 4;  // A tab is visualized as 4 space characters


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
	codeEditor = new QTextEdit(this);

	QFont font("Courier", 10);
	font.setFixedPitch(true);
	font.setStyleHint(QFont::Monospace);
	codeEditor->setFont(font);

	// Make tabs the same size than 4 spaces
	QFontMetrics metrics(font);
	codeEditor->setTabStopWidth(tabStop * metrics.width(' '));

//	highlighter = new Highlighter(editor->document());

	setWidget(codeEditor);
}
