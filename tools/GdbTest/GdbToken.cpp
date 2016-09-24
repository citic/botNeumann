#include "GdbToken.h"

static const struct { GdbToken::Type type; const char* const text; } tokenTypeTextMap[] =
{
	{ GdbToken::UNKNOWN, "unknown" },
	{ GdbToken::C_STRING, "c_string" },
	{ GdbToken::KEY_EQUAL, "=" },
	{ GdbToken::KEY_LEFT_BRACE, "{" },
	{ GdbToken::KEY_RIGHT_BRACE, "}" },
	{ GdbToken::KEY_LEFT_BAR, "[" },
	{ GdbToken::KEY_RIGHT_BAR, "]" },
	{ GdbToken::KEY_UP, "^" },
	{ GdbToken::KEY_PLUS, "+" },
	{ GdbToken::KEY_COMMA, "," },
	{ GdbToken::KEY_TILDE, "~" },
	{ GdbToken::KEY_SNABEL, "@" },
	{ GdbToken::KEY_STAR, "*" },
	{ GdbToken::KEY_AND, "&" },
	{ GdbToken::END_CODE, "endcode" },
	{ GdbToken::VAR, "var" },
};

#define arr_size(arr) (sizeof(arr) / sizeof(arr[0]))

const char* GdbToken::mapTypeToString(GdbToken::Type type)
{
	for ( size_t index = 0; index < arr_size(tokenTypeTextMap); ++index )
		if ( type == tokenTypeTextMap[index].type )
			return tokenTypeTextMap[index].text;

	return nullptr;
}

GdbToken::GdbToken(Type type, const QString& text)
	: type(type)
	, text(text)
{
}

QList<GdbToken*> GdbToken::tokenize(const QString& line)
{
	enum { IDLE, END_CODE, STRING, VAR} parsingState = IDLE;

	QList<GdbToken*> tokenList;
	GdbToken* currentToken = nullptr;
	QChar previousChar = ' ';

	if( line.isEmpty() )
		return tokenList;

	for( int index = 0; index < line.size(); ++index )
	{
		QChar currentChar = line[index];

		switch(parsingState)
		{
			case IDLE:
			{
				if ( currentChar == '"' )
				{
					currentToken = new GdbToken(GdbToken::C_STRING);
					tokenList.push_back(currentToken);
					parsingState = STRING;
				}
				else if ( currentChar == '(' )
				{
					currentToken = new GdbToken(GdbToken::END_CODE, currentChar);
					tokenList.push_back(currentToken);
					parsingState = END_CODE;
				}
				else if ( mapTokenType(currentChar) != UNKNOWN ) // "={}[],^+ ~@&*"
				{
					currentToken = new GdbToken(mapTokenType(currentChar), currentChar);
					tokenList.push_back(currentToken);
					parsingState = IDLE;
				}
				else if ( currentChar != ' ' )
				{
					currentToken = new GdbToken(GdbToken::VAR, currentChar);
					tokenList.push_back(currentToken);
					parsingState = VAR;
				}
			} break;

			case END_CODE:
			{
				// GDB finishes a command output with "(gdb)"
				QString codeEndStr = "(gdb)";
				currentToken->text += currentChar;

				if ( currentToken->text.length() == codeEndStr.length() )
				{
					// The "(gdb)" command end was completely read
					parsingState = IDLE;
				}
				else if (currentToken->text.compare(codeEndStr.left(currentToken->text.length())) != 0)
				{
					// GDB did not produced "(gdb)", something else such as "(grr)"
					currentToken->setType(GdbToken::VAR);
					parsingState = IDLE;
				}
			} break;

			case STRING:
			{
				if(previousChar != '\\' && currentChar == '\\')
				{
				}
				else if(previousChar == '\\')
				{
					if(currentChar == 'n')
						currentToken->text += '\n';
					else
						currentToken->text += currentChar;
				}
				else if(currentChar == '"')
					parsingState = IDLE;
				else
					currentToken->text += currentChar;
			} break;

			case VAR:
			{
				if(currentChar == '=' || currentChar == ',' || currentChar == '{' || currentChar == '}')
				{
					--index;
					currentToken->text = currentToken->text.trimmed();
					parsingState = IDLE;
				}
				else
					currentToken->text += currentChar;
			} break;
		}

		previousChar = currentChar;
	}

	if ( currentToken )
	{
		if ( currentToken->getType() == GdbToken::VAR )
			currentToken->text = currentToken->text.trimmed();
	}

	return tokenList;
}

GdbToken::Type GdbToken::mapTokenType(QChar ch)
{
	switch ( ch.toLatin1() )
	{
		case '=': return KEY_EQUAL;
		case '{': return KEY_LEFT_BRACE;
		case '}': return KEY_RIGHT_BRACE;
		case '[': return KEY_LEFT_BAR;
		case ']': return KEY_RIGHT_BAR;
		case ',': return KEY_COMMA;
		case '^': return KEY_UP;
		case '+': return KEY_PLUS;
		case '~': return KEY_TILDE;
		case '@': return KEY_SNABEL;
		case '&': return KEY_AND;
		case '*': return KEY_STAR;
		default : return UNKNOWN;
	}
}
