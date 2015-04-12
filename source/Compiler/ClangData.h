#ifndef CLANGDATA_H
#define CLANGDATA_H

#ifdef CLANG_INTEGRATED

extern "C"
{
#include <clang-c/Index.h>
}

struct ClangData
{
	// Set of translation units (parsed source files) that may link into an executable or library.
	// May be many translation units in an index
	CXIndex index;
	// Basic unit of compilation in C++. Is a single source file plus any header files directly or
	// indirectly included
	CXTranslationUnit translationUnit;

  public:
	/// Constructor
	ClangData();
	/// Destructor
	~ClangData();
};

#else // CLANG_INTEGRATED

struct ClangData
{
};

#endif // CLANG_INTEGRATED

#endif // CLANGDATA_H
