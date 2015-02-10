#include "Unit.h"
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

Unit::Unit(QObject *parent) : QObject(parent)
{
}

Unit::~Unit()
{
}

bool Unit::load(const QString& filename)
{
	qDebug() << "Unit: loading " << filename;
	QFile file(filename);
	if ( ! file.open(QFile::ReadOnly | QFile::Text) )
	{
		qDebug() << "Unit: could not open " << filename;
		return false;
	}

	QXmlStreamReader xmlReader;
	xmlReader.setDevice(&file);

	loadDocument(xmlReader);

	return xmlReader.error() == false;
}

void Unit::print()
{
	qDebug() << "name:" << name << "id:" << id << "version:" << version << "ram:" << ram << "timeout:" << timeout;
	for ( Descriptions::const_iterator itr = descriptions.begin(); itr != descriptions.end(); ++itr )
		qDebug() << "description lang:" << itr.key() << "value:" << itr.value();
	qDebug() << "initial-code:" << initialCode;
	for ( int i = 0; i < solutions.size(); ++i )
		qDebug() << "solution:" << solutions[i];
	qDebug() << "generator:" << generator;
	for ( int i = 0; i < testCases.size(); ++i )
		qDebug() << "test case input:" << testCases[i].first << "test case output:" << testCases[i].second;
}

bool Unit::loadDocument(QXmlStreamReader& xmlReader)
{
	// Load the document element, it must be <botnu>, example:
	// <botnu name="sum_xy" id="jhc-20150205-205858" version="1.0" ram="512k" timeout="1">
	if ( xmlReader.readNextStartElement() )
	{
		if ( xmlReader.name() == "botnu" )
		{
			// Load attributes for <botnu> root element
			name = xmlReader.attributes().value("name").toString();
			id = xmlReader.attributes().value("id").toString();
			version = xmlReader.attributes().value("version").toString();
			ram = xmlReader.attributes().value("ram").toString();
			timeout = xmlReader.attributes().value("timeout").toInt();

			// Root element is done. Load the child elements
			while ( xmlReader.readNextStartElement() )
			{
				// xmlREader has found a new element, load it according to its tag name
				if ( xmlReader.name() == "description" )
					descriptions.insert(xmlReader.attributes().value("lang").toString(), xmlReader.readElementText());
				else if ( xmlReader.name() == "initial-code" )
					initialCode = xmlReader.readElementText();
				else if ( xmlReader.name() == "solution" )
					solutions.append( xmlReader.readElementText() );
				else if ( xmlReader.name() == "generator" )
					generator = xmlReader.readElementText();
				else if ( xmlReader.name() == "test-case" )
					loadTestCase(xmlReader);
				else
					qDebug() << "Unit: ignoring element:" << xmlReader.name();
			}
		}
		else
			return false;
	}
	return true;
}

bool Unit::loadTestCase(QXmlStreamReader& xmlReader)
{
	QString input, output;

	// An input element is mandatory
	if ( xmlReader.readNextStartElement() && xmlReader.name() == "input" )
		input = xmlReader.readElementText();
	else return false;

	// Followed by an output element
	if ( xmlReader.readNextStartElement() && xmlReader.name() == "output" )
		output = xmlReader.readElementText();
	else return false;

	testCases.append( QPair<QString, QString>(input, output) );
	return true;
}
