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
	// Open (trunk) or create the target file
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
