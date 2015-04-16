#include "CompilerDiagnostic.h"
#include <QFileInfo>
#include <QRegularExpressionMatch>

CompilerDiagnostic::CompilerDiagnostic(const QRegularExpressionMatch& match)
{
	// The expression used to know if a line starts a new error/warning was:
	// " (1filepath)   (2lin) (3col)  (4sev)  (5br)
	// "^(\\w?:?[^:]+):(\\d+):(\\d+): (\\w+): (.+)$"
	// "/path/to/sourcefile.cpp:line:col: severity: short diagnostic message"
	severity = mapSeverityText( match.captured(4) );
	filename = match.captured(1);
	line = match.captured(2).toUInt();
	column = match.captured(3).toUInt();
	brief = match.captured(5);
}

CompilerDiagnostic::~CompilerDiagnostic()
{
}

QString CompilerDiagnostic::buildUserText() const
{
	// To extract only the file name, instead of the complete path
	QFileInfo fileInfo( filename );

	//toolsOutput->insertHtml(QString("<p class='%1'>%2:%3:%4: %5: %6</p>\n\n")
	return QString("%1:%2:%3: %4: %5\n%6")
		.arg(fileInfo.fileName())
		.arg(line)
		.arg(column)
		.arg(getSeverityText())
		.arg(brief)
		.arg(explanation);
}
