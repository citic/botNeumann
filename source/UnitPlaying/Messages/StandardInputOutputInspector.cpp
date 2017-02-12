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
	standardInputInspector->setStyleSheet("background: rgb(222,222,255);");
	addWidget(standardInputInspector);

	toolBar = new QToolBar("Options");
	toolBar->setOrientation(Qt::Vertical);
	setStretchFactor(1, 0);

	toggleOutputErrorAction = new QAction(QIcon(":/unit_playing/buttons/input_output.svg"), tr("Toggle output/error"), this);
	toggleOutputErrorAction->setCheckable(true);
	toggleOutputErrorAction->setChecked(false);
	connect( toggleOutputErrorAction, SIGNAL(toggled(bool)), this, SLOT(toggleOutputError()) );
	toolBar->addAction(toggleOutputErrorAction);
	addWidget(toolBar);

	standardOutputInspector = new QTextEdit();
	standardOutputInspector->setReadOnly(true);
	standardOutputInspector->setStyleSheet("background: rgb(222,255,222);");
	addWidget(standardOutputInspector);

	standardErrorInspector = new QTextEdit();
	standardErrorInspector->setReadOnly(true);
	standardErrorInspector->setStyleSheet("background: rgb(255,222,222);");
	standardErrorInspector->setVisible(false);
	addWidget(standardErrorInspector);
}

void StandardInputOutputInspector::toggleOutputError()
{
	bool checked = toggleOutputErrorAction->isChecked();
	standardOutputInspector->setVisible( ! checked );
	standardErrorInspector->setVisible( checked );
}
