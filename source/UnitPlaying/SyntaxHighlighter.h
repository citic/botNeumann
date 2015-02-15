#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QTextDocument;

class SyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
	Q_DISABLE_COPY(SyntaxHighlighter)

  protected:
	/// Formatting rules consist of a regular expresion and a style. Each time a text mataches
	/// the regular expresion, the corresponding format is applied to the text
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
		HighlightingRule() { }
		HighlightingRule(const QString& pattern, const QTextCharFormat& format) : pattern(pattern), format(format) { }
	};
	/// There are several rules: for keywords, preprocessor, operators, etc.
	QList<HighlightingRule> highlightingRules;
	/// Style for /* multi-line comments */
	QTextCharFormat multiLineCommentFormat;
	/// Regular expression to identify the beginning of /* multi-line comments
	const QRegExp commentStartExpression;
	/// Regular expression to identify the end of multi-line comments */
	const QRegExp commentEndExpression;

  public:
	/// Constructor
	explicit SyntaxHighlighter(QTextDocument* parent);
	/// Destructor
	~SyntaxHighlighter();

  protected:
	/// Create rules for C++ keywords and add them to the rules container
	void createKeywordRules();
	/// Create rules for special elements, such as strings, characters, numbers, functions, ...
	void createSpecialRules();
	/// Create rules for C-style and C++ comments
	void createCommentRules();
	/// Called each time the QTextEditor requires to apply color to a block of code
	virtual void highlightBlock(const QString &text) override;
	/// Apply the store rules on the list highlightingRules to the given text
	void highlightRules(const QString &text);
	/// Apply special rules to /* multi-line comments */
	void highlightMultiLineComments(const QString &text);
};

#endif // SYNTAXHIGHLIGHTER_H
