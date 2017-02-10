#include "BotNeumannApp.h"
#include "CompiledProgram.h"
#include "Compiler.h"
#include "Diagnostic.h"
#include "LogManager.h"
#include "MessagesArea.h"

#include <QListWidget>
#include <QTabWidget>
#include <QTextEdit>

MessagesArea::MessagesArea(QWidget* parent, Qt::WindowFlags flags)
	: QDockWidget("Messages", parent, flags)
	, compiler(nullptr)
{
	// QMainWindow requires all its direct children have a name to store preferences
	setObjectName("messagesDockWidget");

	// Each message control is placed within a tab control
	messagesTabWidget = new QTabWidget(this);
	setWidget(messagesTabWidget);

	// Add the first tab (page): the unit problem description
	unitDescription = new QTextEdit();
	unitDescription->setReadOnly(true);
	unitDescription->document()->setDefaultFont(BotNeumannApp::getRobotFont());
	unitDescription->document()->setDefaultStyleSheet("pre { margin-left: 1em; background: #ddd; }");
	QIcon unitDescriptionIcon(":/button_information.svg");
	messagesTabWidget->addTab(unitDescription, unitDescriptionIcon, tr("Description"));

	// Add the second tab: output generated by build and debug tools
	toolsOutput = new QListWidget();
	toolsOutput->setFont( BotNeumannApp::getMonospacedFont() );
	toolsOutput->setWordWrap(true);
	toolsOutput->setTextElideMode(Qt::ElideNone); // Avoid ellipsis for large texts
	connect(toolsOutput, SIGNAL(currentRowChanged(int)), this, SLOT(toolsOutputRowChanged(int)));
	//toolsOutput->setStyleSheet("QListWidget::item { border-bottom: 1px dashed gray; }");
	QIcon toolsOutputIcon(":/unit_playing/buttons/info.svg");
	messagesTabWidget->addTab(toolsOutput, toolsOutputIcon, tr("Compile output"));

	// Some messages received by the LogManager may be shown in the GUI
	LogManager::setMessagesArea(this);

	// ToDo: Add a third tab: for player's solution input/output
}

MessagesArea::~MessagesArea()
{
	LogManager::setMessagesArea(nullptr);
}

void MessagesArea::setUnitDescription(const QString &description, bool makeActiveTab)
{
	unitDescription->setHtml(description);
	if ( makeActiveTab )
		messagesTabWidget->setCurrentWidget(unitDescription);
}

void MessagesArea::clear()
{
	// A new build process has started. Output from previous compiler calls are outdated, clear them
	Q_UNUSED(compiler);
	this->compiler = nullptr;
	toolsOutput->clear();
}

void MessagesArea::playerSolutionBuilt(CompiledProgram* compiledProgram)
{
	// Save the compiler to get information of the diagnostics when user select them
	this->compiler = compiledProgram->getCompiler();

	// If there are output generated by the compiler, show it
	const QList<Diagnostic*>& diagnostics = compiler->getAllDiagnostics();
	if ( diagnostics.size() > 0 )
	{
		// Show each diagnostic
		for ( int i = 0; i < diagnostics.size(); ++i )
			appendDiagnostic(diagnostics[i]);

		// If this messages dock widget is hidden, make it visible and the tools output tab active
		this->setVisible(true);
		messagesTabWidget->setCurrentWidget(toolsOutput);
	}

	// Always print a result
	int errors = compiler->getErrorCount();
	int warnings = compiler->getWarningCount();
	const QString& text = tr("Build finished: %1 error(s), %2 warning(s)").arg(errors).arg(warnings);
	new QListWidgetItem(text, toolsOutput);

	// If no errors, the debug output is appended, change the widget title
	if ( errors == 0 )
		messagesTabWidget->setTabText( 1, tr("Debugger output") );
}

void MessagesArea::appendDiagnostic(const Diagnostic* diagnostic)
{
	Q_ASSERT(diagnostic);

	// Create a list item with icon according to the severity of the diagnostic.
	// The text may be multiline. The constructor adds the item to the list widget automatically
	QIcon icon( diagnostic->getSeverityIcon() );
	QListWidgetItem* listItem = new QListWidgetItem(icon, diagnostic->buildUserText(), toolsOutput);

	// Provide some styles
	listItem->setForeground( diagnostic->getSeverityColor() );

	static size_t messageCount = 0;
	qCInfo(logBuild, "/%zu/[%s]", ++messageCount, qPrintable(diagnostic->buildUserText()) );
}

void MessagesArea::appendDebuggerMessage(QtMsgType type, const QString& category, const QString& message)
{
	QColor color = Qt::black;
	QIcon icon;

	if ( category == "ADbR" )
	{
		color = Qt::black;
		icon.addFile(":/unit_playing/buttons/monitor.svg");
	}
	else if ( category == "ADbC")
	{
		color = Qt::darkGreen;
		icon.addFile(":/unit_playing/buttons/info.svg");
	}
	else
	{
		switch ( type )
		{
			case QtWarningMsg: color = Qt::magenta; icon.addFile(":/unit_playing/buttons/warning.svg"); break;
			case QtFatalMsg:
			case QtCriticalMsg: color = Qt::red; icon.addFile(":/unit_playing/buttons/error.svg"); break;
			case QtDebugMsg: color = Qt::darkGray; icon.addFile(":/unit_playing/buttons/link_broken.svg"); break;
			default: icon.addFile(":/unit_playing/buttons/info.svg"); break;
		}
	}

	QListWidgetItem* listItem = new QListWidgetItem(icon, message, toolsOutput);

	// Provide some styles
	listItem->setForeground( color );
}

void MessagesArea::toolsOutputRowChanged(int row)
{
	// If there are not diagnostics, or selected the summary line, ignore the event
	if ( compiler && row >= 0 && row < compiler->getAllDiagnostics().size() )
		emit diagnosticSelected(row);
}
