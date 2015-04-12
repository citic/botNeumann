#include "Diagnostic.h"

#ifdef CLANG_INTEGRATED

extern "C"
{
#include <clang-c/Index.h>
}

#endif // CLANG_INTEGRATED

// User friendly text maps to the severity codes in English
static const char* diagnosticSeverityText[] =
{
	"ignored",
	"note",
	"warning",
	"error",
	"fatal"
};

#ifdef CLANG_INTEGRATED

inline QString clangStringToQt(CXString cxString)
{
	QString result( clang_getCString(cxString) );
	clang_disposeString(cxString);
	return result;
}

Diagnostic::Diagnostic(CXDiagnostic diagnostic)
	: severity{ static_cast<DiagnosticSeverity>( clang_getDiagnosticSeverity(diagnostic) ) }
	, line{0}
	, column{0}
	, message{ clangStringToQt( clang_getDiagnosticSpelling(diagnostic) ) }
	, category{ clangStringToQt( clang_getDiagnosticCategoryText(diagnostic) ) }
{
	// The file, line number and character number where the warning/error seems to be produced
	CXSourceLocation location = clang_getDiagnosticLocation(diagnostic);

	// Extract each part from the location structure
	CXString sourceFilename;
	clang_getPresumedLocation(location, &sourceFilename, &line, &column);

	// Convert the filename to Qt string and release the Clang string memory
	filename = clangStringToQt(sourceFilename);
}

#else // CLANG_INTEGRATED

Diagnostic::Diagnostic(CXDiagnostic diagnostic)
	: severity{ DiagnosticSeverity::ignored }
	, line{0}
	, column{0}
{
	Q_UNUSED(diagnostic);
}

#endif // CLANG_INTEGRATED

Diagnostic::~Diagnostic()
{
}

QString Diagnostic::getSeverityText() const
{
	return diagnosticSeverityText[static_cast<int>(severity)];
}
