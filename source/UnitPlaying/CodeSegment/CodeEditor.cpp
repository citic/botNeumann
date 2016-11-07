#include "BotNeumannApp.h"
#include "CodeEditor.h"
#include "Common.h"
#include "GuiBreakpoint.h"
#include "LineNumberArea.h"
#include "Player.h"
#include "PlayerSolution.h"
#include "SyntaxHighlighter.h"
#include "Unit.h"

#include <QDebug>
#include <QDir>
#include <QFile>
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
	, playerSolution(nullptr)
	, autoSaveTimer( new QTimer(this) )
	, lineNumberArea( new LineNumberArea(this) )
	, synchronizedWithObjectCode( false )
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
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));

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

void CodeEditor::setPlayerSolutionForUnit(Unit* unit, PlayerSolution* playerSolution)
{
	this->unit = unit;
	this->playerSolution = playerSolution;
}

bool CodeEditor::loadInitialFile(const QString& filepath)
{
	// Store the active unit and the filepath for future use
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
	document()->setModified(true);

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

	// ToDo: restore existing breakpoints from PlayerSolution or configuration

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
	int lineNumberAreaWidth = getLineNumberAreaWidth();
	int fontHeight = fontMetrics().height() - 1;

	// Adjust these values by the height of the current text block in each iteration in the loop
	while (block.isValid() && top <= event->rect().bottom())
	{
		// Only update numbers for visible blocks, not for hidden blocks, even when they are hidded
		// by another window
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen( QColor(Qt::gray).darker() );
			// If this line has a breakpoint, paint it
			paintBreakpoint(block, painter, top, lineNumberAreaWidth, fontHeight);
			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
		}

		// Move to the nex block (complete line)
		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

void CodeEditor::paintBreakpoint(QTextBlock& block, QPainter& painter, int top, int width, int fontHeight)
{
	GuiBreakpoint* breakpoint = dynamic_cast<GuiBreakpoint*>( block.userData() );
	if ( breakpoint == nullptr ) return;

	QColor color = breakpoint->isSyncWithObjectCode(block.blockNumber() + 1) ? Qt::red : Qt::magenta;
	painter.fillRect(0, top, width, fontHeight, color.lighter() );
}

void CodeEditor::toggleBreakpointEvent(QMouseEvent* event)
{
	// Locate the line where user made click. Each line should be analyzed one by one because
	// word-wrap is enabled by default and some blocks may occupy two lines
	QTextBlock block = firstVisibleBlock();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();
	int clickedY = event->pos().y();

	// Adjust these values by the height of the current text block in each iteration in the loop
	while ( block.isValid() && block.isVisible() )
	{
		// Check if clicked pixel is inside of this block
		if ( clickedY >= top && clickedY < bottom )
		{
			// Updates the set of breakpoints
			toggleBreakpoint(block);
			// Asks Qt to schedule a update event to paint or erase the breakpoint marker
			lineNumberArea->update(0, top, getLineNumberAreaWidth(), fontMetrics().height());
		}

		// Move to the nex block (complete line)
		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
	}
}

void CodeEditor::toggleBreakpoint(QTextBlock& block)
{
	// Check if this line has an associated breakpoint
	GuiBreakpoint* guiBreakpoint = dynamic_cast<GuiBreakpoint*>( block.userData() );
	if ( guiBreakpoint )
	{
		// Update the breakpoint number in the interface. If this number is distinct to the
		// line number on the last build, the breakpoint is not synchronized
		guiBreakpoint->updateLineNumber(block.blockNumber() + 1, synchronizedWithObjectCode);
		// This breakpont object will indicate the receiver it is a delettion of the breakpoint
		guiBreakpoint->setAction(GuiBreakpoint::Action::removed);
		// If visualization is running, ask debugger to clear the breakpoint
		emit breakpointAction(guiBreakpoint);
		// The line already has a breakpoint, remove it
		block.setUserData(nullptr);
	}
	else
	{
		// User is trying to create a new breakpoint
		GuiBreakpoint* guiBreakpoint = new GuiBreakpoint( filepath, block.blockNumber() + 1);
		// If source code is synchronized with object code, is it safe to set breakpoints
		if ( synchronizedWithObjectCode )
			guiBreakpoint->setLineNumberInObjectCode( block.blockNumber() + 1 );
		// Store the breakpoint as user data in the code editor
		block.setUserData(guiBreakpoint);
		// This breakpont object will indicate the receiver it is a delettion of the breakpoint
		guiBreakpoint->setAction(GuiBreakpoint::Action::created);
		// If visualization is running, ask debugger to set the breakpoint
		emit breakpointAction(guiBreakpoint);
	}
}

QList<GuiBreakpoint*> CodeEditor::retrieveBreakpoints()
{
	this->synchronizedWithObjectCode = true;

	// A list of pairs "source:lineNumber" of breakpoints
	QList<GuiBreakpoint*> result;

	// Iterate for all the lines in this document finding breakpoints
	for ( QTextBlock block = document()->begin(); block != document()->end(); block = block.next() )
	{
		// If this line has a breakpoint, convert it to a string in "file:lineNumber" format
		GuiBreakpoint* guiBreakpoint = dynamic_cast<GuiBreakpoint*>( block.userData() );
		if ( guiBreakpoint )
		{
			guiBreakpoint->updateLineNumber( block.blockNumber() + 1, true );
			result.append( guiBreakpoint );
		}
	}

	return result;
}

void CodeEditor::updateLineNumberAreaWidth()
{
	this->synchronizedWithObjectCode = false;
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

	QTextCursor cur( document()->findBlockByLineNumber(line) );
	cur.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, column);
	setTextCursor(cur);
	setFocus();

	// ToDo: change the color of the number in the line number area
	//lineNumberArea
}
