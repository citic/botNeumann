#include "BotNeumannApp.h"
#include "CodeEditorDockWidget.h"
#include "SyntaxHighlighter.h"
#include <QAction>
#include <QMainWindow>
#include <QTextEdit>
#include <QToolBar>

// A tab is visualized as 3 space characters because screen size is reduced in the game
const int tabStop = 3;

CodeEditorDockWidget::CodeEditorDockWidget(QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(tr("Program"), parent, flags)
	, innerMainWindow( new QMainWindow(this) )
{
	setObjectName("codeEditor");

	setupInnerWidget();
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

void CodeEditorDockWidget::setupInnerWidget()
{
	// Ask the main window to behave as normal widget, instead of a real main window
	innerMainWindow->setWindowFlags(Qt::Widget);

	// Set the inner main window as the inner widget of this dock widget
	setWidget(innerMainWindow);
}

void CodeEditorDockWidget::setupToolbar()
{
	// Create the toolbar
	QToolBar* toolBar = innerMainWindow->addToolBar("code");

	// Create the Run or pause action
	runOrPauseAction = new QAction(QIcon(":/unit_playing/unit_playing/button_run.svg"), tr("&Run or pause"), this);
	runOrPauseAction->setShortcut(QKeySequence("Ctrl+R"));
	runOrPauseAction->setStatusTip(tr("Compiles the code and starts its visualization"));
	connect(runOrPauseAction, SIGNAL(triggered()), this, SLOT(runOrPauseTriggered()));
	toolBar->addAction(runOrPauseAction);

	// Create the step into button
	stepIntoAction = new QAction(QIcon(":/unit_playing/unit_playing/button_step_into.svg"), tr("Step &into"), this);
	stepIntoAction->setShortcut(QKeySequence("Ctrl+I"));
	stepIntoAction->setStatusTip(tr("Executes next statement entering in functions"));
	stepIntoAction->setEnabled(false);
	connect(stepIntoAction, SIGNAL(triggered()), this, SLOT(stepIntoTriggered()));
	toolBar->addAction(stepIntoAction);

/* Svg missing for the moment
	// Create the step out button
	stepOutAction = new QAction(QIcon(":/unit_playing/unit_playing/button_step_out.svg"), tr("Step &out"), this);
	stepOutAction->setShortcut(QKeySequence("Ctrl+O"));
	stepOutAction->setStatusTip(tr("Executes next statement in current function"));
	stepOutAction->setEnabled(false);
	connect(stepOutAction, SIGNAL(triggered()), this, SLOT(stepOutTriggered()));
	toolBar->addAction(stepOutAction);
*/
	// Create the stop button
	stopAction = new QAction(QIcon(":/unit_playing/unit_playing/button_stop.svg"), tr("S&top"), this);
	stopAction->setShortcut(QKeySequence("Ctrl+T"));
	stopAction->setStatusTip(tr("Compiles the code and starts its visualization"));
	stopAction->setEnabled(false);
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stopTriggered()));
	toolBar->addAction(stopAction);
}

void CodeEditorDockWidget::setupCodeEditor()
{
	// The code editor is a QTextEdit object
	codeEditor = new QTextEdit(this);

	// Set the default monospaced font of the operating system
//	QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	QFont font(BotNeumannApp::getMonospacedFontName());
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
	innerMainWindow->setCentralWidget(codeEditor);
}

#include <QDebug>

void CodeEditorDockWidget::runOrPauseTriggered()
{
	qDebug() << "Run or pause triggered";
}

void CodeEditorDockWidget::stepIntoTriggered()
{
	qDebug() << "Step into triggered";
}

void CodeEditorDockWidget::stepOutTriggered()
{
	qDebug() << "Step out triggered";
}

void CodeEditorDockWidget::stopTriggered()
{
	qDebug() << "Stop triggered";
}
