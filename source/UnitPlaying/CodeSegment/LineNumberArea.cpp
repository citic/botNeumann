#include "LineNumberArea.h"
#include "CodeEditor.h"

LineNumberArea::LineNumberArea(CodeEditor* codeEditor)
	: QWidget(codeEditor)
	, codeEditor(codeEditor)
{
}

LineNumberArea::~LineNumberArea()
{
}

QSize LineNumberArea::sizeHint() const
{
	return QSize(codeEditor->getLineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
	codeEditor->lineNumberAreaPaintEvent(event);
}
