#include "BotNeumannApp.h"
#include "CodeEditor.h"
#include "Common.h"
#include "LineNumberArea.h"
#include "Player.h"
#include "SyntaxHighlighter.h"
#include "Unit.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QTimer>

// A tab is visualized as 3 space characters because screen size is reduced in the game
const int tabStop = 3; // characters

// After a change is made to the document, CodeEditor waits this amount of miliseconds, and then
// autosaves and autocompiles the source code
const int autoSaveWait = 2500; // milliseconds

// Number of pixels to leave as breakpoint area in the line number widget
// I set it to 0. I think it is better to mark the number with some other color
const int breakpointEdgeWidth = 0; // pixels

CodeEditor::CodeEditor(QWidget* parent)
	: QPlainTextEdit(parent)
	, unit(nullptr)
	, autoSaveTimer( new QTimer(this) )
	, lineNumberArea( new LineNumberArea(this) )
{
	// Set the default monospaced font of the game
	const QFont& font = BotNeumannApp::getMonospacedFont();
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

	// When user changes the number of lines on the editor, adjust the area of the line number
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));

	// When this editor must be updated, because another window was covering it, or something else
	// update the line number area widget too
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

	// When user changes the cursor, highlight the active line and un-paint the old active one
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	// Calculate the initial line number area width
	updateLineNumberAreaWidth();

	// Make the first line as the the active one
	highlightCurrentLine();
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
	// If there is a document with changes, save it
	if ( filepath.length() > 0) saveChanges();

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

	// If there is a pending autosave operation, cancel it
	autoSaveTimer->stop();
	qDebug() << "CodeEditor: File saved:" << filepath;
	return true;
}

// Line number area methods

void CodeEditor::resizeEvent(QResizeEvent* event)
{
	// When the size of the editor changes, we also need to resize the line number area.
	QPlainTextEdit::resizeEvent(event);

	// Set to the line number area the same position and dimensions of the code editor, except
	// its width. The width of the line number depends on the number of lines
	const QRect& cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), getLineNumberAreaWidth(), cr.height()));
}

int CodeEditor::getLineNumberAreaWidth()
{
	// Get the number of lines currently displayed in the document
	// QPlainTextEdit uses the term block instead of line. If word wrap is enabled, a long line is
	// automatically sliced  and called a block (QTextBlock), compound of several virtual lines.
	int max = qMax(1, blockCount());

	// Count the digits required by that number
	int digits = digitsRequiredBy(max);

	// Calculate the maximum width in pixels of a digit
	int digitWidth = fontMetrics().width(QLatin1Char('9'));

	// The width in pixels of the line number area is the width of the last line number
	return digits * digitWidth + breakpointEdgeWidth;
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
	// Painting the line number widget's background as ligth gray
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	// We are going to loop through all visible lines, start with the first one.
	// We get the top and bottom y-coordinate of the first text block
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	// Adjust these values by the height of the current text block in each iteration in the loop
	while (block.isValid() && top <= event->rect().bottom())
	{
		// Only update numbers for visible blocks, not for hidden blocks, even when they are hidded
		// by another window
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen( QColor(Qt::gray).darker() );
			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
		}

		// Move to the nex block (complete line)
		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

void CodeEditor::updateLineNumberAreaWidth()
{
	// Make room in the left edge of the editor
	setViewportMargins(getLineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
	// User scrolled the text editor, scroll the line number area also
	if (dy)
		lineNumberArea->scroll(0, dy);
	else
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

	if ( rect.contains(viewport()->rect()) )
		updateLineNumberAreaWidth();
}

void CodeEditor::highlightCurrentLine()
{
	// QPlainTextEdit gives the possibility to have more than one selection at the same time.
	// We can set the character format (QTextCharFormat) of these selections.
	QList<QTextEdit::ExtraSelection> extraSelections;

	if ( ! isReadOnly() )
	{
		QTextEdit::ExtraSelection selection;

		// The style for the active line is light yellow and it is applied to the entire line/block
		QColor lineColor = QColor(Qt::yellow).lighter(160);
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);

		// We clear the cursors selection before setting the new new QPlainTextEdit::ExtraSelection,
		// else several lines would get highlighted when the user selects multiple lines with the mouse.
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::placeCursor(int line, int column)
{
	// Change the cursor only if line is valid
	if ( line < 0 || line >= blockCount() )
		return;

	QTextCursor cur( document()->findBlockByLineNumber(line - 1) );
	cur.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, column);
	setTextCursor(cur);
	setFocus();

	// ToDo: change the color of the number in the line number area
	//lineNumberArea
}
