#include "Diagnostic.h"

extern "C"
{
#include <clang-c/Index.h>
}

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

Diagnostic::~Diagnostic()
{
}
