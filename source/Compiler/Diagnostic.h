#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <QString>

// Taken from <clang-c/Index.h>
typedef void *CXDiagnostic;

// Adapted from <clang-c/Index.h>
enum class DiagnosticSeverity
{
	ignored, // A diagnostic that has been suppressed, e.g., by a command-line
	note, // A note that should be attached to the previous (non-note) diagnostic
	warning, // Indicates suspicious code that may not be wrong
	error, // indicates that the code is ill-formed
	fatal // error such that future parser recovery is unlikely to produce useful results
};

/**
	@brief A warning or error found by the LLVM/Clang compiler
 */
class Diagnostic
{
  protected:
	/// A enum value indicating if it is a warning, error or fatal error
	DiagnosticSeverity severity;
	/// The filename where the warning or error was found
	QString filename;
	/// The line where the warning or error is presumed to be produced
	unsigned line;
	/// The column where the warning or error is presumed to be produced
	unsigned column;
	/// An user friendly explanation of the diagnostic (warning or error)
	QString message;
	/// Generated by lexer, syntax, semantic
	QString category;

  public:
	/// Constructor
	explicit Diagnostic(CXDiagnostic diagnostic);
	/// Destructor
	~Diagnostic();
};

#endif // DIAGNOSTIC_H