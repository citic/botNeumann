#include "GdbToken.h"

GdbToken::GdbToken(Type type, const QString& text)
	: type(type)
	, text(text)
{
}

QList<GdbToken*> GdbToken::tokenize(const QString& line)
{
	enum { IDLE, END_CODE, STRING, VAR} state = IDLE;

	QList<GdbToken*> list;
	GdbToken *cur = NULL;
	QChar prevC = ' ';

	if(line.isEmpty())
		return list;

	for(int i = 0;i < line.size();i++)
	{
		QChar c = line[i];
		switch(state)
		{
			case IDLE:
			{
				if(c == '"')
				{
					cur = new GdbToken(GdbToken::C_STRING);
					list.push_back(cur);
					state = STRING;
				}
				else if(c == '(')
				{
					cur = new GdbToken(GdbToken::END_CODE);
					list.push_back(cur);
					cur->text += c;
					state = END_CODE;
				}
				else if(c == '=' || c == '{' || c == '}' || c == ',' ||
					c == '[' || c == ']' || c == '+' || c == '^' ||
					c == '~' || c == '@' || c == '&' || c == '*')
				{
					GdbToken::Type type = GdbToken::UNKNOWN;
					if(c == '=')
						type = GdbToken::KEY_EQUAL;
					if(c == '{')
						type = GdbToken::KEY_LEFT_BRACE;
					if(c == '}')
						type = GdbToken::KEY_RIGHT_BRACE;
					if(c == '[')
						type = GdbToken::KEY_LEFT_BAR;
					if(c == ']')
						type = GdbToken::KEY_RIGHT_BAR;
					if(c == ',')
						type = GdbToken::KEY_COMMA;
					if(c == '^')
						type = GdbToken::KEY_UP;
					if(c == '+')
						type = GdbToken::KEY_PLUS;
					if(c == '~')
						type = GdbToken::KEY_TILDE;
					if(c == '@')
						type = GdbToken::KEY_SNABEL;
					if(c == '&')
						type = GdbToken::KEY_AND;
					if(c == '*')
						type = GdbToken::KEY_STAR;
					cur = new GdbToken(type);
					list.push_back(cur);
					cur->text += c;
					state = IDLE;
				}
				else if( c != ' ')
				{
					cur = new GdbToken(GdbToken::VAR);
					list.push_back(cur);
					cur->text = c;
					state = VAR;
				}

			};break;
			case END_CODE:
			{
				QString codeEndStr = "(gdb)";
				cur->text += c;

				if(cur->text.length() == codeEndStr.length())
				{
					state = IDLE;

				}
				else if(cur->text.compare(codeEndStr.left(cur->text.length())) != 0)
				{
					cur->setType(GdbToken::VAR);
					state = IDLE;
				}

			};break;
			case STRING:
			{
				if(prevC != '\\' && c == '\\')
				{
				}
				else if(prevC == '\\')
				{
					if(c == 'n')
						cur->text += '\n';
					else
						cur->text += c;
				}
				else if(c == '"')
					state = IDLE;
				else
					cur->text += c;
			};break;
			case VAR:
			{
				if(c == '=' || c == ',' || c == '{' || c == '}')
				{
					i--;
					cur->text = cur->text.trimmed();
					state = IDLE;
				}
				else
					cur->text += c;
			};break;

		}
		prevC = c;
	}
	if(cur)
	{
		if(cur->getType() == GdbToken::VAR)
			cur->text = cur->text.trimmed();
	}
	return list;
}
