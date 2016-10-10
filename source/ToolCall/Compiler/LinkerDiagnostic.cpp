#include "LinkerDiagnostic.h"

LinkerDiagnostic::LinkerDiagnostic(const QString& text)
{
	// By default, all linker diagnostics are considered errors
	// Except when the word warning is found in the text
	severity = text.contains("warning") ? DiagnosticSeverity::warning : DiagnosticSeverity::error;

	// Usually it begins with a file, but for simplicity in this version all the text
	// is stored with no changes
	brief = text;
}

LinkerDiagnostic::~LinkerDiagnostic()
{
}

QString LinkerDiagnostic::buildUserText() const
{
	return brief;
}

QColor LinkerDiagnostic::getSeverityColor() const
{
	return Qt::magenta;
}

QIcon LinkerDiagnostic::getSeverityIcon() const
{
	return QIcon(":/unit_playing/buttons/link_broken.svg");
}
