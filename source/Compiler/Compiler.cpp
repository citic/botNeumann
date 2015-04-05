#include "Compiler.h"
#include "ClangData.h"
#include "Diagnostic.h"

Compiler::Compiler(QObject *parent)
	: QObject(parent)
	, clangData(nullptr)
{
}

Compiler::~Compiler()
{
	clear();
}

void Compiler::compile(const QString& filename)
{
	// Clear old compilation contexts
	clear();
	// Create a compilation context for this file
	clangData = new ClangData();

	// The given file by parameter will be compiled as any other file. We can pass compiler
	// arguments using an C-style array. We send default include directories as an example
	const char* cxxArgs[] =
	{
		"-I/usr/include",
		"-I."
	};

	// Number of command line arguments to be sent to the compiler
	const int cxxArgCount = sizeof(cxxArgs) / sizeof(cxxArgs[0]);

	// Compile the given file, a CLang translation unit will be generated in dynamic memory
	clangData->translationUnit = clang_parseTranslationUnit(clangData->index, filename.toStdString().c_str(), cxxArgs, cxxArgCount, nullptr, 0, CXTranslationUnit_None);

	// Get the number of warnings and errors detected by Clang and make store space for them
	unsigned diagnosticCount = clang_getNumDiagnostics(clangData->translationUnit);
	diagnostics.reserve(diagnosticCount);

	// Iterate through all warnings and errors found by Clang and fill the diagnostics container
	for (unsigned i = 0; i < diagnosticCount; ++i)
	{
		// Get the diagnostic created in dynamic memory
		CXDiagnostic diagnostic = clang_getDiagnostic(clangData->translationUnit, i);

		// Create an object Diagnostic and extract information from the Clang diagnostic
		diagnostics.append( new Diagnostic(diagnostic) );

		// Release the dynamic memory required by the Clang diagnostic
		clang_disposeDiagnostic(diagnostic);
	}
}

void Compiler::compile(const QStringList& filenames)
{
	// Not implemented yet
	Q_ASSERT(false);
	Q_UNUSED(filenames);
}

void Compiler::clear()
{
	for (int i = 0; i < diagnostics.size(); ++i)
		delete diagnostics[i];

	diagnostics.clear();
	delete clangData;
	clangData = nullptr;
}
