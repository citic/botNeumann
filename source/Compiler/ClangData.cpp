#include "ClangData.h"

ClangData::ClangData()
	: index(clang_createIndex(false, false))
	, translationUnit(nullptr)
{
}

ClangData::~ClangData()
{
	// The translation unit and the index were created in dynamic memory, release it
	clang_disposeTranslationUnit(translationUnit);
	clang_disposeIndex(index);
}
