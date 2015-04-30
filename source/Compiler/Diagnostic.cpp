#include "Diagnostic.h"
#include <QRegularExpression>

// User friendly text maps to the severity codes in English
static const char* diagnosticSeverityText[] =
{
	"ignored",
	"note",
	"warning",
	"error",
	"fatal",
	"unknown"
};

Diagnostic::Diagnostic()
	: severity(DiagnosticSeverity::unknown)
	, line(-1)
	, column(-1)
{
}

Diagnostic::~Diagnostic()
{
}

QString Diagnostic::getSeverityText() const
{
	return diagnosticSeverityText[static_cast<int>(severity)];
}

DiagnosticSeverity Diagnostic::mapSeverityText(const QString& text)
{
	int count = sizeof(diagnosticSeverityText) / sizeof(diagnosticSeverityText[0]);
	for ( int i = 0; i < count; ++i )
		if ( text == diagnosticSeverityText[i] )
			return static_cast<DiagnosticSeverity>(i);

	return DiagnosticSeverity::unknown;
}

QColor Diagnostic::mapSeverityColor(DiagnosticSeverity severity)
{
	switch ( severity )
	{
		case DiagnosticSeverity::ignored: return Qt::lightGray;
		case DiagnosticSeverity::note:    return Qt::gray;
		case DiagnosticSeverity::warning: return Qt::blue;
		case DiagnosticSeverity::error:   return Qt::red;
		case DiagnosticSeverity::fatal:   return Qt::red;
		case DiagnosticSeverity::unknown: return Qt::black;
        default: Q_ASSERT(false); return Qt::black;
	}
}

QIcon Diagnostic::mapSeverityIcon(DiagnosticSeverity severity)
{
	switch ( severity )
	{
		case DiagnosticSeverity::ignored: return QIcon(":/unit_playing/buttons/info.svg");
		case DiagnosticSeverity::note:    return QIcon(":/unit_playing/buttons/info.svg");
		case DiagnosticSeverity::warning: return QIcon(":/unit_playing/buttons/warning.svg");
		case DiagnosticSeverity::error:   return QIcon(":/unit_playing/buttons/error.svg");
		case DiagnosticSeverity::fatal:   return QIcon(":/unit_playing/buttons/error.svg");
		case DiagnosticSeverity::unknown: return QIcon(":/unit_playing/buttons/info.svg");
        default: Q_ASSERT(false); return QIcon(":/unit_playing/buttons/info.svg");
    }
}

void Diagnostic::appendExplanation(const QString& text)
{
	// Avoid summary messages
	if ( text.contains(QRegularExpression("(warning|error).* generated\\.$")))
		return;

	// If there is previous text, separate by a new line
	if ( explanation.length() > 0 )
		explanation.append('\n');

	explanation.append(text);
}
