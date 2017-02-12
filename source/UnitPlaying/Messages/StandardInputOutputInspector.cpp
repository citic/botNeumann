#include "StandardInputOutputInspector.h"

#include <QACtion>
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
	standardInputInspector->setReadOnly(true);
	standardInputInspector->setStyleSheet("background: rgb(242,242,242);");
	addWidget(standardInputInspector);
	setStretchFactor(0, 1);

	toolBar = new QToolBar("Options");
	toolBar->setOrientation(Qt::Vertical);
	QAction* toggleOutputErrorAction = new QAction(QIcon(":/unit_playing/buttons/input_output.svg"), tr("Toggle output/error"), this);
	toolBar->addAction(toggleOutputErrorAction);
	addWidget(toolBar);
	setStretchFactor(1, 0);

	standardOutputInspector = new QTextEdit();
	standardOutputInspector->setReadOnly(true);
	standardOutputInspector->setStyleSheet("background: rgb(242,242,242);");
	addWidget(standardOutputInspector);
	setStretchFactor(2, 1);

	standardErrorInspector = new QTextEdit();
	standardErrorInspector->setReadOnly(true);
	standardErrorInspector->setStyleSheet("background: rgb(242,242,242);");
	standardErrorInspector->setVisible(false);
	addWidget(standardErrorInspector);
	setStretchFactor(3, 1);
}

void StandardInputOutputInspector::toggleOutputError()
{
}
