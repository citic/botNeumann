#include "GuiBreakpoint.h"
#include <QFileInfo>

GuiBreakpoint::GuiBreakpoint(const QString& filename, int lineNumberInEditor)
	: filename(filename)
	, lineNumberInEditor(lineNumberInEditor)
	, lineNumberInObjectCode(-1)
	, action( Action::unknown )
{
}

void GuiBreakpoint::updateLineNumber(int lineNumber, bool both)
{
	this->lineNumberInEditor = lineNumber;
	if ( both )
		this->lineNumberInObjectCode = lineNumber;
}

bool GuiBreakpoint::isSyncWithObjectCode(int currentLineNumber)
{
	this->lineNumberInEditor = currentLineNumber;
	return lineNumberInEditor == lineNumberInObjectCode;
}

QString GuiBreakpoint::buildFileLineString() const
{
	return QString("\"%1:%2\"").arg(QFileInfo(filename).fileName()).arg(lineNumberInObjectCode);
}
