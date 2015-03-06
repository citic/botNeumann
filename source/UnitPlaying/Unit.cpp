#include "Unit.h"
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

/// If not ram size is specified in .botnu file, this constant will be assumed
static const int defaultRamSize = 1024; // bytes
/// If not cpu-cores is specified in .botnu file, this constant will be assumed
static const int defaultCpuCores = 1; // bytes

Unit::Unit(QObject* parent)
	: QObject(parent)
	, ramSize(defaultRamSize)
	, heapSegment(false)
	, cpuCores(defaultCpuCores)
	, timeout(0)
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

const QString Unit::getDescription(const QString& language) const
{
	if ( descriptions.contains(language) )
		return descriptions.value(language);
	if ( descriptions.size() > 0 )
		return descriptions.constBegin().value();
	return QString();
}

void Unit::print()
{
	qDebug() << "id:" << id << "version:" << version << "ram:" << ramSize << "heap-segment:" << heapSegment << "cpu-cores:" << cpuCores << "timeout:" << timeout;
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
		if ( xmlReader.name() != "botnu" )
			return false;

		// Load attributes for <botnu> root element
		loadDocumentAttributes(xmlReader);

		// Root element is done. Load the child elements
		while ( xmlReader.readNextStartElement() )
			if ( ! loadDocumentChild(xmlReader)  )
				qDebug() << "Unit: ignoring element:" << xmlReader.name();
	}
	return true;
}

bool Unit::loadDocumentAttributes(QXmlStreamReader& xmlReader)
{
	id = xmlReader.attributes().value("id").toString();
	version = xmlReader.attributes().value("version").toString();
	ramSize = xmlReader.attributes().value("ram").toInt();
	if ( ramSize <= 0 ) ramSize = defaultRamSize;
	heapSegment = xmlReader.attributes().value("heap-segment") == "yes";
	cpuCores = xmlReader.attributes().value("cpu-cores").toInt();
	if ( cpuCores <= 0 ) cpuCores = defaultCpuCores;
	timeout = xmlReader.attributes().value("timeout").toInt();

	// ToDo: validate the attributes values
	return true;
}

bool Unit::loadDocumentChild(QXmlStreamReader& xmlReader)
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
		return false;

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
