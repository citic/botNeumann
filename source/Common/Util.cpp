#include "Util.h"
#include "LogManager.h"

#include <QDir>
#include <QTextStream>

QColor Util::mixColors(const QColor& first, const QColor& second, qreal ratio)
{
	int red = first.red() * ratio + second.red() * (1.0 - ratio);
	int green = first.green() * ratio + second.green() * (1.0 - ratio);
	int blue = first.blue() * ratio + second.blue() * (1.0 - ratio);
	int alpha = first.alpha() * ratio + second.alpha() * (1.0 - ratio);
	return QColor(red, green, blue, alpha);
}

QColor Util::averageColors(const QList<QColor>& colors)
{
	long long red = 0, green = 0, blue = 0, alpha = 0;
	foreach(const QColor& color, colors)
	{
		red += color.red();
		green += color.green();
		blue += color.blue();
		alpha += color.alpha();
	}
	if ( colors.count() > 0 )
	{
		red /= colors.count();
		green /= colors.count();
		blue /= colors.count();
		alpha /= colors.count();
	}
	return QColor(red, green, blue, alpha);
}

bool Util::createDirectory(const QString& dirPath)
{
	QDir dir( dirPath );

	if ( dir.exists() )
		return true;

	return dir.mkpath( "." );
}

bool ResourceToFileDumper::dumpString(const QString& data, const QString& outputFilename)
{
	// Open (truncate) or create the target file
	QFile outputFile(outputFilename);
	if ( outputFile.open(QIODevice::WriteOnly | QIODevice::Text) == false )
	{
		qCritical(logApplication) << "Could not create" << outputFilename;
		return false;
	}

	// Write the data to the target file
	QTextStream outputCode(&outputFile);
	outputCode << data;

	// Destructors will close the file
	return true;
}

bool ResourceToFileDumper::dumpStringList(const QStringList& data, const QString& outputFilename)
{
	// Open (truncate) or create the target file
	QFile outputFile(outputFilename);
	if ( outputFile.open(QIODevice::WriteOnly | QIODevice::Text) == false )
	{
		qCritical(logApplication) << "Could not create" << outputFilename;
		return false;
	}

	// Write the data to the target file
	QTextStream outputText(&outputFile);
	for ( int index = 0; index < data.count(); ++index )
		outputText << data[index] << '\n';

	// Destructors will close the file
	return true;
}

void ResourceToFileDumper::addSearchAndReplace(const QString& search, const QString& replace)
{
	searchAndReplaceRules.append( QPair<QString, QString>(search, replace) );
}

bool ResourceToFileDumper::dumpTextResource(const QString& resource, const QString& outputFilename)
{
	// Get the contents of the source file from a resource
	QFile inputFile(resource);

	// Open the source resource for reading
	if ( inputFile.open(QIODevice::ReadOnly | QIODevice::Text) == false )
	{
		// Log an error and return an invalid file path
		qCritical(logApplication) << "Could not open" << resource;
		return "";
	}

	// Open (truncate) or create the target file
	QFile outputFile(outputFilename);
	if ( outputFile.open(QIODevice::WriteOnly | QIODevice::Text) == false )
	{
		qCritical(logApplication) << "Could not create" << outputFilename;
		return false;
	}

	// Both, source resource and target file are open successfully. We need deal with text
	QTextStream inputCode(&inputFile);
	QTextStream outputCode(&outputFile);

	// Copy each line of the source to the target file, but some search/replacements may be required
	while ( ! inputCode.atEnd() )
	{
		// Get a line from the source resource
		QString line = inputCode.readLine();

		// ToDo: Replace paths to the files that will be used to redirect standard input, output and error
		for ( int index = 0; index < searchAndReplaceRules.count(); ++index )
			line.replace( searchAndReplaceRules[index].first, searchAndReplaceRules[index].second );

		// The line is done, write it to the target file
		outputCode << line << '\n';
	}

	// Destructors will close the files
	return true;
}

QStringList Util::readAllLines(const QString& filepath)
{
	// Get the contents of the source file keeping the lines
	QStringList result;

	// Open the file for reading
	QFile inputFile(filepath);
	if ( inputFile.open(QIODevice::ReadOnly | QIODevice::Text) == true )
	{
		// Copy each line of the source to the string list
		QTextStream inputText(&inputFile);
		while ( ! inputText.atEnd() )
			result.append( inputText.readLine() );
	}
	else
		qCritical(logApplication) << "Could not open" << filepath;

	return result;
}

long Util::findFirstDiff(const QString& filepath1, const QString& filepath2, bool ignoreWhitespace, bool caseSensitive)
{
	// Open the files
	QFile inputFile1(filepath1);
	QFile inputFile2(filepath2);

	if ( inputFile1.open(QIODevice::ReadOnly | QIODevice::Text) == false )
		{ qCritical(logApplication) << "Could not open" << filepath1; return -2; }
	if ( inputFile2.open(QIODevice::ReadOnly | QIODevice::Text) == false )
		{ qCritical(logApplication) << "Could not open" << filepath2; return -2; }

	// If both files are empty, they match
	if ( inputFile1.atEnd() && inputFile2.atEnd() )
		return -1;

	// At least one file has data, we have to read them to compare
	QTextStream inputText1(&inputFile1);
	QTextStream inputText2(&inputFile2);

	// Used to know the current position in files
	long position1 = 0;
	QChar ch1 = 0;
	QChar ch2 = 0;

	// Read until reaching the first EOF
	while ( ! inputText1.atEnd() )
	{
		if ( inputText2.atEnd() )
			return position1;

		// Get the next char of each file
		ch1 = readNextChar(inputText1, ignoreWhitespace, caseSensitive, ch1 == ' ', &position1);
		ch2 = readNextChar(inputText2, ignoreWhitespace, caseSensitive, ch2 == ' ');

		// Compare the next character of each file
		if ( ch1 != ch2 )
			return position1;

		// Both files are identical until this char, continue
	}

	// If there is remaining expected information that player did not produced, fail
	if ( ! inputFile2.atEnd() )
		return position1;

	// We did not find any difference
	return -1;
}

QChar Util::readNextChar(QTextStream& input, bool ignoreWhitespace, bool caseSensitive, bool eatWhitespace, long* position)
{
	// Read the next character
	QChar ch = -1;

	// We may need to repeat when ignoring whitespace
	while ( ! input.atEnd() )
	{
		// Read the character
		input >> ch;

		// We have read a character, advance the position counter
		if ( position )
			++(*position);

		// If we have to ignore case, we convert all characters to lowercase
		if ( caseSensitive == false )
			ch = ch.toLower();

		// If we got a non space character, we have found the next character
		if ( ! ch.isSpace() )
			return ch;

		// We found a whitespace character, if they must be match exact, return it
		if ( ! ignoreWhitespace )
			return ch;

		// We found a whitespace character and are ignoring them, uniform them to spaces
		ch = ' ';

		// If asked to eat whitespaces, we continue the while, otherwise we stop here
		if ( ! eatWhitespace )
			return ch;
	}

	return ch;
}
