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
