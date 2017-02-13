#include "StandardInputOutputInspector.h"
#include "LogManager.h"
#include "PlayerSolution.h"

#include <QACtion>
#include <QTextEdit>
#include <QTextStream>
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

bool StandardInputOutputInspector::loadTestCase(int testCaseNumber, PlayerSolution* playerSolution)
{
	this->testCaseNumber = testCaseNumber;

//	const QString& args      = playerSolution->buildTestCaseFilepath(testCaseNumber, "args");
	const QString& input     = playerSolution->buildTestCaseFilepath(testCaseNumber, "input");
	const QString& output_ex = playerSolution->buildTestCaseFilepath(testCaseNumber, "output_ex");
//	const QString& output_ps = playerSolution->buildTestCaseFilepath(testCaseNumber, "output_ps");
	const QString& error_ex  = playerSolution->buildTestCaseFilepath(testCaseNumber, "error_ex");
//	const QString& error_ps  = playerSolution->buildTestCaseFilepath(testCaseNumber, "error_ps");

	return loadFileInto(input, standardInputInspector)
		&& loadFileInto(output_ex, standardOutputInspector)
		&& loadFileInto(error_ex, standardErrorInspector);
}

bool StandardInputOutputInspector::loadFileInto(const QString& filepath, QTextEdit* inspector)
{
	// Open the files
	QFile inputFile(filepath);
	if ( inputFile.open(QIODevice::ReadOnly | QIODevice::Text) == false )
		{ qCritical(logApplication) << "Could not open" << filepath; return false; }

	// At least one file has data, we have to read them to compare
	Q_ASSERT(inspector);
	inspector->setText( QTextStream(&inputFile).readAll() );
	return true;
}
