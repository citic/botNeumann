#include "BotNeumannApp.h"
#include "CodeEditor.h"
#include "Common.h"
#include "GuiBreakpoint.h"
#include "LineNumberArea.h"
#include "LogManager.h"
#include "Player.h"
#include "PlayerSolution.h"
#include "SyntaxHighlighter.h"
#include "Unit.h"
#include "Util.h"

#include <QDir>
#include <QFile>
#include <QPainter>
#include <QTimer>

// After a change is made to the document, CodeEditor waits this amount of miliseconds, and then
// autosaves and autocompiles the source code
static const int autoSaveWait = 2500; // milliseconds

// Number of pixels to leave as breakpoint area in the line number widget
// I set it to 0. I think it is better to mark the number with some other color
static const int breakpointEdgeWidth = 0; // pixels

// The color to highlight the current line
static const QColor& currentLineColor = QColor(Qt::magenta).lighter(175);


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
	setTabStopDistance( metrics.horizontalAdvance("   ") );

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
	qCInfo(logEditor) << "Editing" << filepath;

	// Clear previous selections
	clearHighlights(false);

	// If exists a source file load it, otherwise load the initial code provided by the unit
	bool result = QFile::exists(filepath) ? loadFileContents() : loadUnitInitialCode();

	// Make the first line as the the active one
	currentLine = -1;
	setTextCursor( QTextCursor( document()->firstBlock() ) );
	highlightCurrentLine();

	return result;
}

bool CodeEditor::loadFile(const QString& filepath)
{
	this->filepath = filepath;
	return loadFileContents();
}

bool CodeEditor::loadUnitInitialCode()
{
	Q_ASSERT(playerSolution);
	const ProgramText* initialCode = playerSolution->getInitialCode();
	if ( initialCode )
		setPlainText( initialCode->code );

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
		qCCritical(logEditor) << "Could not open file" << filepath;
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
			qCritical(logEditor) << "Could not create directory:" << dir.absolutePath();
			return false;
		}

	// ToDo: dot not save if there are not changes, that is, document is not modified
	QFile file(filepath);

	// Open the target file for writing text considering line changes format
	if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		qCritical(logEditor) << "Could not open file for writing:" << filepath;
		return false;
	}

	// Save the current text from textEdit to a text file with UTF-8 codification
	if ( file.write( toPlainText().toUtf8() ) == -1 )
	{
		qCritical(logEditor) << "Could not write file:" << filepath;
		return false;
	}

	// Document was successfully saved, no changes are left
	document()->setModified(false);

	// If there is a pending autosave operation, cancel it
	autoSaveTimer->stop();
	qCInfo(logEditor) << "File saved:" << filepath;
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
	int digitWidth = fontMetrics().horizontalAdvance('9');

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
	int top = static_cast<int>( blockBoundingGeometry(block).translated(contentOffset()).top() );
	int bottom = top + static_cast<int>( blockBoundingRect(block).height() );
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
		bottom = top + static_cast<int>( blockBoundingRect(block).height() );
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
	int top = static_cast<int>( blockBoundingGeometry(block).translated(contentOffset()).top() );
	int bottom = top + static_cast<int>( blockBoundingRect(block).height() );
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
		bottom = top + static_cast<int>( blockBoundingRect(block).height() );
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
	// Do not highlight in animating mode
	if ( animating )
		return;

	// Get the line number where the cursor is
	// Line numbers are 1-based, block numbers are 0-based
	int newCurrentLine = textCursor().blockNumber() + 1;

	// If the cursor is in the same line that is already selected, we do not need to update
	if ( currentLine != newCurrentLine )
	{
		// The cursor changed of line, clear the previous highlighted line
		if ( currentLine > 0 )
			clearHighlight(currentLine, currentLineColor, false);

		// And highlight the new selected line
		currentLine = newCurrentLine;
		addHighlight(newCurrentLine, currentLineColor, true);
	}
}

void CodeEditor::addHighlight(int line, const QColor& backgroundColor, bool updateView)
{
	// Add the highlight to the internal hash
	Q_ASSERT(line > 0);
	lineColors[line].append(backgroundColor);

	// Apply the change on the view, if requested
	if ( updateView )
		updateHighlights();
}

void CodeEditor::clearHighlight(int line, const QColor& backgroundColor, bool updateView)
{
	// Remove the highlight from the internal hash
	Q_ASSERT(line > 0);
	lineColors[line].removeOne(backgroundColor);

	// Apply the change on the view, if requested
	if ( updateView )
		updateHighlights();
}

void CodeEditor::updateHighlights()
{
	// QPlainTextEdit gives the possibility to have more than one selection at the same time.
	// We can set the character format (QTextCharFormat) of these selections.
	QList<QTextEdit::ExtraSelection> extraSelections;

	// Traverse all the lines that have colors
	for (LineColorsType::const_iterator itr = lineColors.constBegin(); itr != lineColors.constEnd(); ++itr )
	{
		// Get the line number and its colors
		int line = itr.key();
		const QList<QColor>& lineColors = itr.value();

		// Merge all the colors for this line
		const QColor& averageColor = Util::averageColors(lineColors);

		// Create a resulting style for this line
		QTextEdit::ExtraSelection selection;

		// Highlight the entire line with the resulting color
		selection.format.setBackground(averageColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);

		// Create a cursor for the line
		selection.cursor = QTextCursor(document()->findBlockByNumber(line - 1));
		selection.cursor.clearSelection();

		// Add the style to be applied to this line with the rest of lines
		extraSelections.append(selection);
	}

	// Apply the highlights to the view
	setExtraSelections(extraSelections);
}

void CodeEditor::clearHighlights(bool keepCurrentLine)
{
	// Remove all mappings between lines and their background colors
	lineColors.clear();

	// Repaint current line if asked
	if ( keepCurrentLine )
		addHighlight(textCursor().blockNumber() + 1, currentLineColor, false);

	// Update the interface
	updateHighlights();
}

void CodeEditor::placeCursor(int line, int column)
{
	// Change the cursor only if line is valid
	if ( line < 0 || line >= blockCount() )
		return;

	// Now move the cursor to the desired position
	QTextCursor cursor( document()->findBlockByLineNumber(line) );
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, column);
	setTextCursor(cursor);
	setFocus();

	// ToDo: change the color of the number in the line number area
	//lineNumberArea
}

void CodeEditor::makeLineVisible(int line)
{
	// If asked, scroll the editor to make the cursor visible
	// This is made using a trick: move the cursor to the end before
	moveCursor(QTextCursor::End);

	// Move the cursor some lines before the attempted line, to give a little context
	placeCursor( qMax(0, line - getVisibleLines() / 2), 0);
}

int CodeEditor::getVisibleLines()
{
	QFontMetrics fontMetrics( font() );
	qreal rowHeight = fontMetrics.lineSpacing();
	return static_cast<int>( width() / rowHeight );
}

void CodeEditor::setAnimating(bool state)
{
	// If no change, do nothing
	if ( animating == state )
		return;

	// State changed. If we are now animating
	animating = state;
	if ( animating )
	{
		saveCursor();
		clearHighlights(false);
	}
	else
	{
		restoreCursor();
		clearHighlights(true);
	}
}
