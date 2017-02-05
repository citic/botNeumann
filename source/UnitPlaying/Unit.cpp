#include "Unit.h"
#include "Common.h"
#include "LogManager.h"

#include <cmath>
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
	DELETE_POINTERS_ARRAY(initialCodes);
	DELETE_POINTERS_ARRAY(solutions);
	DELETE_POINTERS_ARRAY(generators);
}

bool Unit::load(const QString& filename)
{
	qCInfo(logPlayer) << "Unit: loading" << filename;
	QFile file(filename);
	if ( ! file.open(QFile::ReadOnly | QFile::Text) )
	{
		qCDebug(logApplication) << "Unit: could not open " << filename;
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

#define RANDOM_SELECT(list) \
	if ( list.size() > 0 ) \
		return list[ qrand() % list.size() ]; \
	return nullptr;

const ProgramText* Unit::getARandomInitialCode() const
{
	RANDOM_SELECT(initialCodes)
}

const ProgramText* Unit::getARandomSolution() const
{
	RANDOM_SELECT(solutions)
}

const ProgramText* Unit::getARandomGenerator() const
{
	RANDOM_SELECT(generators)
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
				qCDebug(logApplication) << "Unit: ignoring element:" << xmlReader.name();
	}

	return validateUnit();
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
		qCDebug(logApplication) << QString("%1.botnu: insufficient RAM %2B, assumed %3B").arg(id).arg(ramSize).arg(requiredRam);
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
		descriptions.insert( xmlReader.attributes().value("lang").toString(), xmlReader.readElementText() );
	else if ( xmlReader.name() == "initial-code" )
		initialCodes.append( new ProgramText(ProgramText::initialCode, xmlReader)  );
	else if ( xmlReader.name() == "solution" )
		solutions.append( new ProgramText(ProgramText::solution, xmlReader)  );
	else if ( xmlReader.name() == "standard-generator" )
		generators.append( new ProgramText(ProgramText::standardGenerator, xmlReader) );
	else if ( xmlReader.name() == "file-generator" )
		generators.append( new ProgramText(ProgramText::fileGenerator, xmlReader)  );
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

bool Unit::validateUnit()
{
	// A description is mandatory
	if ( descriptions.count() <= 0 )
	{
		qCCritical(logApplication).noquote() << "Unit" << id << "has no any description";
		return false;
	}

	// An initial code is mandatory
	if ( initialCodes.count() <= 0 )
	{
		qCCritical(logApplication).noquote() << "Unit" << id << "has no any initial code";
		return false;
	}

	// At least an input is mandatory
	if ( testCases.count() <= 0 && generators.count() <= 0 )
	{
		qCCritical(logApplication).noquote() << "Unit" << id << "has no test cases nor generators";
		return false;
	}

	// Count the number of standard generators provided
	int standardGeneratorCount = 0;
	for ( int index = 0; index < generators.count(); ++index )
		if ( generators[index]->type == ProgramText::standardGenerator )
			++standardGeneratorCount;

	// If there are standard generators, a least one solution must be provided
	if ( standardGeneratorCount > 0 && solutions.count() <= 0 )
	{
		qCCritical(logApplication).noquote() << "Unit" << id << "has standard generators but no solutions";
		return false;
	}

	return true;
}

void Unit::distributeMemory()
{
	// Total number of columns. It is the same for all segments
	double k = sqrt(28.0 * ramSize / (45.0 * largestDataTypeSize));
	columns = size_t(k / cpuCores) * cpuCores;

	// Visible rows of the stack segment
	stackSegmentVisibleRows = sqrt(ramSize / (35.0 * largestDataTypeSize));
}

bool ProgramText::load(QXmlStreamReader& xmlReader)
{
	language = xmlReader.attributes().value("lang").toString();
	if ( language.isEmpty() ) language = "cpp";

	defaultRuns = xmlReader.attributes().value("default-runs").toInt();
	if ( defaultRuns == 0 ) defaultRuns = 10;

	code = xmlReader.readElementText();
	return true;
}
