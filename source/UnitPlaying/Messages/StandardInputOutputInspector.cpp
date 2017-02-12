#include "StandardInputOutputInspector.h"

#include <QTextEdit>
#include <QToolBar>

StandardInputOutputInspector::StandardInputOutputInspector(QWidget *parent)
	: QSplitter(parent)
{
	buildInspector();
}

void StandardInputOutputInspector::buildInspector()
{
	standardInputInspector = new QTextEdit();
	addWidget(standardInputInspector);

	toolBar = new QToolBar("Options");
	addWidget(toolBar);

	standardOutputInspector = new QTextEdit();
	addWidget(standardOutputInspector);

	standardErrorInspector = new QTextEdit();
	standardErrorInspector->setVisible(false);
	addWidget(standardErrorInspector);
}
