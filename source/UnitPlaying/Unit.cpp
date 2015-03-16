#include "Unit.h"
#include <cmath>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

/// If not architecture is specified in .botnu file, this constant will be assumed
static const int defaultArchitecture = 32; // bits
/// Supported architectures by botNeumann
static const int supportedArchitectures[] = {16, 32, 64};
/// If not cpu-cores is specified in .botnu file, this constant will be assumed
static const int defaultCpuCores = 4;
/// If not ram size is specified in .botnu file, this constant will be assumed
static const size_t defaultRamSize = 1024; // bytes
/// If not 'min-threads' is specified in .botnu file, this constant will be assumed
static const int defaultMinThreads = 1;
/// If not 'max-threads' is specified in .botnu file, this constant will be assumed
static const int defaultMaxThreads = 0;
/// If not 'timeout' is specified in .botnu file, this constant will be assumed
static const int defaultTimeout = 1; // seconds

Unit::Unit(QObject* parent)
	: QObject(parent)
	, architecture(defaultArchitecture)
	, cpuCores(defaultCpuCores)
	, ramSize(defaultRamSize)
	, heapSegment(false)
	, minThreads(defaultMinThreads)
	, maxThreads(defaultMaxThreads)
	, timeout(0)
	, ignoreWhitespace(true)
	, columns(0)
	, stackSegmentVisibleRows(0)
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
	distributeMemory();

	return xmlReader.error() == false;
}

bool Unit::isArchitectureSupported(int bits)
{
	for (size_t i = 0; i < sizeof(supportedArchitectures)/sizeof(supportedArchitectures[0]); ++i)
		if ( bits == supportedArchitectures[i] )
			return true;

	return false;
}

const QString Unit::getDescription(const QString& language) const
{
	if ( descriptions.contains(language) )
		return descriptions.value(language);
	if ( descriptions.size() > 0 )
		return descriptions.constBegin().value();
	return QString();
}

QString Unit::getARandomInitialCode() const
{
	return initialCodes.size() > 0 ? initialCodes[qrand() % initialCodes.size()] : QString();
}

QString Unit::getARandomSolution() const
{
	return solutions.size() > 0 ? solutions[qrand() % solutions.size()] : QString();
}

QString Unit::getARandomGenerator() const
{
	return generators.size() > 0 ? generators[qrand() % generators.size()] : QString();
}

void Unit::print()
{
	qDebug() << "id:" << id << "version:" << version << "ram:" << ramSize << "heap-segment:" << heapSegment << "cpu-cores:" << cpuCores << "min-theads:" << minThreads << "timeout:" << timeout;
	for ( Descriptions::const_iterator itr = descriptions.begin(); itr != descriptions.end(); ++itr )
		qDebug() << "description lang:" << itr.key() << "value:" << itr.value();
	qDebug() << "initial-code:" << initialCodes;
	for ( int i = 0; i < solutions.size(); ++i )
		qDebug() << "solution:" << solutions[i];
	qDebug() << "generator:" << generators;
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
	// Id and versions
	id = xmlReader.attributes().value("id").toString();
	version = xmlReader.attributes().value("version").toString();
	botnuVersion = xmlReader.attributes().value("botnu-version").toString();

	// Architecture
	architecture = xmlReader.attributes().value("architecture").toInt();
	if ( ! isArchitectureSupported(architecture) ) architecture = defaultArchitecture;

	// CPU cores
	cpuCores = xmlReader.attributes().value("cpu-cores").toInt();
	if ( cpuCores <= 0 ) cpuCores = defaultCpuCores;

	// RAM size, it must be at least max(45/28*a*c^2, 256) bytes
	size_t requiredRam = 45 * largestDataTypeSize * cpuCores * cpuCores / 28 + 1;
	if ( requiredRam < 256 ) requiredRam = 256;
	ramSize = xmlReader.attributes().value("ram").toULongLong();
	if ( ramSize < requiredRam )
	{
		qDebug() << QString("%1.botnu: insufficient RAM %2B, assumed %3B").arg(id).arg(ramSize).arg(requiredRam);
		ramSize = requiredRam;
	}

	// Use heap segment?
	heapSegment = xmlReader.attributes().value("heap-segment") != "no";

	// Execution threads
	minThreads = xmlReader.attributes().value("min-threads").toInt();
	if ( minThreads <= 0 ) minThreads = defaultMinThreads;
	maxThreads = xmlReader.attributes().value("max-threads").toInt();
	if ( maxThreads < 0 ) maxThreads = defaultMaxThreads;

	// Timeout
	timeout = xmlReader.attributes().value("timeout").toInt();
	if ( timeout < 0 ) timeout = defaultTimeout;

	// Ignore whitespace when comparing output
	ignoreWhitespace = xmlReader.attributes().value("ignore-whitespace") != "no";

	// ToDo: validate the attributes values
	return true;
}

bool Unit::loadDocumentChild(QXmlStreamReader& xmlReader)
{
	// xmlREader has found a new element, load it according to its tag name
	if ( xmlReader.name() == "description" )
		descriptions.insert(xmlReader.attributes().value("lang").toString(), xmlReader.readElementText());
	else if ( xmlReader.name() == "initial-code" )
		initialCodes.append( xmlReader.readElementText() );
	else if ( xmlReader.name() == "solution" )
		solutions.append( xmlReader.readElementText() );
	else if ( xmlReader.name() == "generator" )
		generators.append( xmlReader.readElementText() );
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

void Unit::distributeMemory()
{
	// Total number of columns. It is the same for all segments
	double k = sqrt(28.0 * ramSize / (45.0 * largestDataTypeSize));
	columns = size_t(k / cpuCores) * cpuCores;

	// Visible rows of the stack segment
	stackSegmentVisibleRows = sqrt(ramSize / (35.0 * largestDataTypeSize));
	qDebug() << "Stack segment visible rows" << stackSegmentVisibleRows << "of" << columns / cpuCores << "columns each";
}
