#include "Diagnostic.h"
#include <QRegularExpressionMatch>

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

Diagnostic::Diagnostic(const QRegularExpressionMatch& match)
	: severity{ mapSeverityText( match.captured(4) ) }
	, filename{ match.captured(1) }
	, line{ match.captured(2).toUInt() }
	, column{ match.captured(3).toUInt() }
	, brief{ match.captured(5) }
{
	// The expression used to know if a line starts a new error/warning was:
	// " (1filepath)   (2lin) (3col)  (4sev)  (5br)
	// "^(\\w?:?[^:]+):(\\d+):(\\d+): (\\w+): (.+)$"
	// "/path/to/sourcefile.cpp:line:col: severity: short diagnostic message"
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

void Diagnostic::appendExplanation(const QString& text)
{
	explanation.append(text);
	explanation.append('\n');
}
