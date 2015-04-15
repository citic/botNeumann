#include "BotNeumannApp.h"
#include "CodeEditor.h"
#include "Player.h"
#include "SyntaxHighlighter.h"
#include "Unit.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTimer>

// A tab is visualized as 3 space characters because screen size is reduced in the game
const int tabStop = 3;

// Default font point size
#ifdef Q_OS_WIN
const int editorFontSize = 10;
#else
const int editorFontSize = 11;
#endif

// After a change is made to the document, CodeEditor waits this amount of miliseconds, and then
// autosaves and autocompiles the source code
const int autoSaveWait = 2500;

CodeEditor::CodeEditor(QWidget* parent)
	: QTextEdit(parent)
	, unit(nullptr)
	, autoSaveTimer( new QTimer(this) )
{
	// Set the default monospaced font of the operating system
//	QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	QFont font(BotNeumannApp::getMonospacedFontName());
	font.setPointSize(editorFontSize);
	font.setStyleHint(QFont::TypeWriter);
	font.setFixedPitch(true);
	setFont(font);

	// Make tabs the same size than 4 spaces
	QFontMetrics metrics(font);
	setTabStopWidth(tabStop * metrics.width(' '));

	// Create the object that will provide color to C++ code within the editor
	highlighter = new SyntaxHighlighter( document() );

	// The idle timer always work in single shot basics
	autoSaveTimer->setSingleShot(true);
	autoSaveTimer->setInterval(autoSaveWait);
	connect(autoSaveTimer, SIGNAL(timeout()), this, SLOT(saveChanges()));
}

CodeEditor::~CodeEditor()
{
	saveChanges();
}

bool CodeEditor::loadFile(Unit* unit, const QString& filepath)
{
	// Store the active unit and the filepath for future use
	this->unit = unit;
	this->filepath = filepath;
	qDebug() << "CodeEditor: editing" << filepath;

	// If exists a source file load it, otherwise load the initial code provided by the unit
	return QFile::exists(filepath) ? loadFileContents() : loadUnitInitialCode();
}

bool CodeEditor::loadFile(const QString& filepath)
{
	this->filepath = filepath;
	return loadFileContents();
}

bool CodeEditor::loadUnitInitialCode()
{
	Q_ASSERT(unit);
	setPlainText( unit->getARandomInitialCode() );
	document()->setModified(false);

	// Each time the document is changed, update the pending time to autosave/autocompile
	connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentChanged()));

	return true;
}

bool CodeEditor::loadFileContents()
{
	// Open the file with Text flag, each line-change character in Windows or Mac format
	// will be converted to '\n' because Qt follows internally the Unix convertions
	QFile file(filepath);
	if ( ! file.open(QIODevice::Text | QIODevice::ReadOnly) )
	{
		qCritical() << "CodeEditor: could not open file" << filepath;
		return false;
	}

	// Read all contents from the file into a sequence of bytes, then, convert the sequence of
	// bytes into a QString object assuming the UTF-8 character encoding (the default for this game)
	setPlainText( QString::fromUtf8(file.readAll()) );
	document()->setModified(false);

	// Each time the document is changed, update the pending time to autosave/autocompile
	connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentChanged()));

	return true;
}

void CodeEditor::documentChanged()
{
	autoSaveTimer->start();
}

bool CodeEditor::saveChanges()
{
	return document()->isModified() ? save() : true;
}

bool CodeEditor::save()
{
	// If there is not file, ignore the call
	if ( filepath.isEmpty() ) return false;

	// If the directory where the file will be stored does not exist, create it
	QFileInfo fileInfo(filepath);
	QDir dir = fileInfo.absoluteDir();
	if ( ! dir.exists() )
		if ( ! dir.mkpath(".") )
		{
			qCritical() << "CodeEditor: Could not create directory:" << dir.absolutePath();
			return false;
		}

	// ToDo: dot not save if there are not changes, that is, document is not modified
	QFile file(filepath);

	// Open the target file for writing text considering line changes format
	if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		qCritical() << "CodeEditor: Could not open file for writing:" << filepath;
		return false;
	}

	// Save the current text from textEdit to a text file with UTF-8 codification
	if ( file.write( toPlainText().toUtf8() ) == -1 )
	{
		qCritical() << "CodeEditor: Could not write file:" << filepath;
		return false;
	}

	// Document was successfully saved, no changes are left
	document()->setModified(false);
	qDebug() << "CodeEditor: File saved:" << filepath;
	return true;
}
