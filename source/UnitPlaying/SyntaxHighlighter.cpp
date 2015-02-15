#include "SyntaxHighlighter.h"

static const int notInComment = 0;
static const int inComment = 1;

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
	: QSyntaxHighlighter(parent)
	, commentStartExpression("/\\*")
	, commentEndExpression("\\*/")
{
	createKeywordRules();
	createSpecialRules();
	createCommentRules();
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

// List of reserved words in C++11 according to http://en.cppreference.com/w/cpp/keyword
static const char* const cppKeywords[] =
{
	"alignas",
	"alignof",
	"and",
	"and_eq",
	"asm",
	"auto",
	"bitand",
	"bitor",
	"bool",
	"break",
	"case",
	"catch",
	"char",
	"char16_t",
	"char32_t",
	"class",
	"compl",
	"concept",
	"const",
	"constexpr",
	"const_cast",
	"continue",
	"decltype",
	"default",
	"delete",
	"do",
	"double",
	"dynamic_cast",
	"else",
	"enum",
	"explicit",
	"export",
	"extern",
	"false",
	"float",
	"for",
	"friend",
	"goto",
	"if",
	"inline",
	"int",
	"long",
	"mutable",
	"namespace",
	"new",
	"noexcept",
	"not",
	"not_eq",
	"nullptr",
	"operator",
	"or",
	"or_eq",
	"private",
	"protected",
	"public",
	"register",
	"reinterpret_cast",
	"requires",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"static_assert",
	"static_cast",
	"struct",
	"switch",
	"template",
	"this",
	"thread_local",
	"throw",
	"true",
	"try",
	"typedef",
	"typeid",
	"typename",
	"union",
	"unsigned",
	"using",
	"virtual",
	"void",
	"volatile",
	"wchar_t",
	"while",
	"xor",
	"xor_eq"
};

void SyntaxHighlighter::createKeywordRules()
{
	// Style for all keywords
	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(Qt::blue);
	keywordFormat.setFontWeight(QFont::Bold);

	// Add a rule for each C++ keyword
	size_t count = sizeof(cppKeywords) / sizeof(cppKeywords[0]);
	for ( size_t i = 0; i < count; ++i )
		highlightingRules.append( HighlightingRule(QString("\\b%1\\b").arg(cppKeywords[i]), keywordFormat) );
}

void SyntaxHighlighter::createSpecialRules()
{
	// Literal strings
	QTextCharFormat quotationFormat;
	quotationFormat.setForeground(QColor(255, 102, 0)); // orange
	highlightingRules.append(HighlightingRule("\".*\"", quotationFormat));

	// Literal characters
	QTextCharFormat charFormat;
	charFormat.setForeground(Qt::darkCyan);
	highlightingRules.append(HighlightingRule("'.*'", charFormat));

	// Functions or methods
	QTextCharFormat functionFormat;
	functionFormat.setForeground(Qt::darkMagenta);
	highlightingRules.append(HighlightingRule("\\b[A-Za-z0-9_]+(?=\\()", functionFormat));

	// Preprocessor
	QTextCharFormat preprocessorFormat;
	preprocessorFormat.setFontWeight(QFont::Bold);
	preprocessorFormat.setForeground(QColor(0, 130, 130)); // teal
	highlightingRules.append(HighlightingRule("#[^\n]*", preprocessorFormat));

	// C++ standard library
	QTextCharFormat stdFormat;
//	stdFormat.setFontWeight(QFont::Bold);
	stdFormat.setForeground(QColor(89, 62, 26)); // brown
	highlightingRules.append(HighlightingRule("\\bstd:?:?[A-Za-z0-9_]*\\b", stdFormat));

}

void SyntaxHighlighter::createCommentRules()
{
	// Single line comments are indentical to other styles
	QTextCharFormat singleLineCommentFormat;
	singleLineCommentFormat.setForeground(Qt::darkGreen);
	highlightingRules.append(HighlightingRule("//[^\n]*", singleLineCommentFormat));

	// According to Qt Syntax Highlighter example: Multiline comment needs special care due to the
	// design of the QSyntaxHighlighter class. After a QSyntaxHighlighter object is created, its
	// highlightBlock() function will be called automatically whenever it is necessary by the rich
	// text engine, highlighting the given text block. The problem appears when a comment spans
	// several text blocks. This problem is managed in highlightBlock() implementation.
	multiLineCommentFormat.setForeground(Qt::darkGreen);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
	// This implementation is adapted from Qt Highlighter example
	highlightRules(text);
	highlightMultiLineComments(text);
}

void SyntaxHighlighter::highlightRules(const QString& text)
{
	// Apply the syntax highlighting rules that we stored in the highlightingRules vector.
	foreach (const HighlightingRule& rule, highlightingRules)
	{
		// For each rule we search for the pattern in the given textblock
		int index = rule.pattern.indexIn(text);
		while (index >= 0)
		{
			// An occurrence of the pattern was found.
			// QRegExp::matchedLength() function determines the string that will be formatted and
			// returns the length of the last matched string, or -1 if there was no match.
			int length = rule.pattern.matchedLength();
			// Apply the format to the matching string between index and index+length
			setFormat(index, length, rule.format);
			// repeat until the last occurrence of the pattern in the current text block is found
			index = rule.pattern.indexIn(text, index + length);
		}
	}
}

void SyntaxHighlighter::highlightMultiLineComments(const QString& text)
{
	// To deal with constructs that can span several text blocks (like the C++ multiline comment),
	// it is necessary to know the end state of the previous text block (e.g. "in comment").
	// QSyntaxHighlighter::previousBlockState() provides the end state of the previous text block
	// After parsing the block you can save its state calling setCurrentBlockState()
	// States are identified with integers, default is -1 meaning unknown state
	setCurrentBlockState(notInComment);

	// If we know we are not inside a comment opened from a previous block, search for the beginning
	// of potential /* multi-line comment
	int startIndex = 0;
	if (previousBlockState() != inComment)
		startIndex = commentStartExpression.indexIn(text);

	// If startIndex is -1 we know there is not comments in this block, and stop
	// But if startIndex is >= 0 there is an open comment
	while (startIndex >= 0)
	{
		// Search the end of the current open multi-line comment */
		int endIndex = commentEndExpression.indexIn(text, startIndex);
		int commentLength;
		if (endIndex == -1)
		{
			// The end of the multi-line comment */ is not in this block, maybe in a posterior one
			// Indicate that this block finished with an open /* multi-line comment
			setCurrentBlockState(inComment);
			commentLength = text.length() - startIndex;
		}
		else
		{
			// We found the end of the multi-line comment */, calculate the length of the comment
			commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
		}

		// Apply the comment style to the not-finished or closed comment
		setFormat(startIndex, commentLength, multiLineCommentFormat);
		// Analyze code after the comment
		startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}
