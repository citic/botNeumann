#include "BotNeumannApp.h"
#include "CodeEditor.h"
#include "Player.h"
#include "SyntaxHighlighter.h"
#include "Unit.h"
#include <QDebug>
#include <QFile>

// A tab is visualized as 3 space characters because screen size is reduced in the game
const int tabStop = 3;

CodeEditor::CodeEditor(QWidget* parent)
	: QTextEdit(parent)
	, unit(nullptr)
{
	// Set the default monospaced font of the operating system
//	QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	QFont font(BotNeumannApp::getMonospacedFontName());
	font.setPointSize(11);
	font.setStyleHint(QFont::TypeWriter);
	font.setFixedPitch(true);
	setFont(font);

	// Make tabs the same size than 4 spaces
	QFontMetrics metrics(font);
	setTabStopWidth(tabStop * metrics.width(' '));

	// Create the object that will provide color to C++ code within the editor
	highlighter = new SyntaxHighlighter( document() );
}

CodeEditor::~CodeEditor()
{
}

bool CodeEditor::loadCodeForUnit(Unit* unit, const QString& filepath)
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

void CodeEditor::reset()
{
	unit = nullptr;
}

bool CodeEditor::loadUnitInitialCode()
{
	setPlainText( unit->getARandomInitialCode() );
	return true;
}

bool CodeEditor::loadFileContents()
{
	// Open the file with Text flag, each line-change character in Windows or Mac format
	// will be converted to '\n' because Qt follows internally the Unix convertions
	QFile file(filepath);
	if ( ! file.open(QIODevice::Text | QIODevice::ReadOnly) )
	{
		qDebug() << "CodeEditor: could not open file" << filepath;
		return false;
	}

	// Read all contents from the file into a sequence of bytes, then, convert the sequence of
	// bytes into a QString object assuming the UTF-8 character encoding (the default for this game)
	setPlainText( QString::fromUtf8(file.readAll()) );
	return true;
}
