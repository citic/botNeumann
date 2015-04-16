#ifndef COMPILERDIAGNOSTIC_H
#define COMPILERDIAGNOSTIC_H

#include "Diagnostic.h"

class QRegularExpressionMatch;

/**
	@brief A warning or error message generated by the compiler
 */
class CompilerDiagnostic : public Diagnostic
{
  protected:
	/// The line where the warning or error is presumed to be produced
	unsigned line;
	/// The column where the warning or error is presumed to be produced
	unsigned column;

  public:
	/// Constructor
	CompilerDiagnostic(const QRegularExpressionMatch& match);
	/// Destructor
	~CompilerDiagnostic();
	/// The line where the warning or error is presumed to be produced
	inline unsigned getLine() const { return line; }
	/// The column where the warning or error is presumed to be produced
	inline unsigned getColumn() const { return column; }
	/// Generates a text line to the user, in order to understand the diagnostic
	virtual QString buildUserText() const override;
};

#endif // COMPILERDIAGNOSTIC_H