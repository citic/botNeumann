#ifndef GDBTOKEN_H
#define GDBTOKEN_H

#include <QList>
#include <QString>

class GdbToken
{
  public:
	enum Type
	{
		UNKNOWN,
		C_STRING,
		KEY_EQUAL,
		KEY_LEFT_BRACE,
		KEY_RIGHT_BRACE,
		KEY_LEFT_BAR,
		KEY_RIGHT_BAR,
		KEY_UP,
		KEY_PLUS,
		KEY_COMMA,
		KEY_TILDE,
		KEY_SNABEL,
		KEY_STAR,
		KEY_AND,
		END_CODE,
		VAR
	};

  protected:
	Type type;
	QString text;

  public:
	explicit GdbToken(Type type, const QString& text = "");
	static QList<GdbToken*> tokenize(const QString& line);
	inline void appendText(QChar ch) { text += ch; }
	inline void setType(Type type) { this->type = type; }
	inline Type getType() const { return this->type; }
	inline const QString& getText() const { return this->text; }
	static Type mapTokenType(QChar ch);
};

#endif // GDBTOKEN_H
