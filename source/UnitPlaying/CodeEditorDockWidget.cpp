#include "CodeEditorDockWidget.h"
#include <QTextEdit>

CodeEditorDockWidget::CodeEditorDockWidget(QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(tr("Program"), parent, flags)
{
	setObjectName("codeEditor");

	// ToDo: only for testing: an empty widget
	setWidget(new QTextEdit());
}

CodeEditorDockWidget::~CodeEditorDockWidget()
{
}
