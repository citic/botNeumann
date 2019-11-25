#include "Unit.h"
#include "Util.h"
#include "Common.h"
#include "LogManager.h"

#include <cmath>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QXmlStreamReader>

/// If not architecture is specified in .botnu file, this constant will be assumed
static const int defaultArchitecture = 32; // bits
/// Supported architectures by botNeumann
static const int supportedArchitectures[] = {16, 32, 64};
/// If not cpu-cores is specified in .botnu file, this constant will be assumed
static const int defaultCpuCores = 2;
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
	, minThreads(defaultMinThreads)
	, maxThreads(defaultMaxThreads)
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
	QFileInfo fileInfo(filename);

	if ( filename.startsWith(':') )
		return loadFromResource(filename);

	if ( fileInfo.isDir() )
		return loadFromFolder(filename);

	QDir dir = fileInfo.dir();
	if ( dir.exists() )
		return loadFromFolder(dir);

	return false;
}

bool Unit::loadFromResource(const QString& filename)
{
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

	if ( xmlReader.error() != QXmlStreamReader::Error::NoError )
	{
		qCCritical(logApplication) << "XML Error:" << xmlReader.errorString();
		return false;
	}

	return true;
}

bool Unit::loadFromFolder(const QString& directory)
{
	QDir dir(directory);
	return this->loadFromFolder(dir);
}

bool Unit::loadFromFolder(QDir dir)
{
	if ( ! dir.exists() )
		return false;

	this->id = dir.dirName();
	dir.setFilter(QDir::Files | QDir::Hidden); /*  | QDir::NoSymLinks */
	dir.setSorting(QDir::Name);
	QRegularExpression reInput("^input\\d+");

	const QFileInfoList& list = dir.entryInfoList();
	for ( int index = 0; index < list.size(); ++index)
	{
		const QFileInfo& fileInfo = list.at(index);
		const QString& filename = fileInfo.fileName();
		const QString& extension = fileInfo.suffix();

		if ( reInput.match(filename).hasMatch() )
			loadTestCase(fileInfo);
		else if ( filename.startsWith("problem") && extension == "html" )
			loadDescription(fileInfo);
		else if ( filename.startsWith("given") && (extension == "c" || extension == "cpp") )
			loadProgramText(fileInfo, initialCodes, ProgramText::initialCode);
		else if ( filename.startsWith("solution") && (extension == "c" || extension == "cpp") )
			loadProgramText(fileInfo, solutions, ProgramText::solution);
		else if ( filename.startsWith("generator") && (extension == "c" || extension == "cpp") )
			loadProgramText(fileInfo, generators, ProgramText::standardGenerator);
		else if ( filename.startsWith("file_generator") && (extension == "c" || extension == "cpp") )
			loadProgramText(fileInfo, generators, ProgramText::fileGenerator);
#if 0
		else if ( extension == "att" )
			loadUnitAtributes(absolute);
#endif
	}

	distributeMemory();
	validateUnit();
	return true;
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

const ProgramText* Unit::getARandomInitialCode() const
{
	if ( initialCodes.size() > 0 )
		return initialCodes[ qrand() % initialCodes.size() ];
	return nullptr;
}

const ProgramText* Unit::getARandomSolution() const
{
	if ( solutions.size() > 0 )
		return solutions[ qrand() % solutions.size() ];
	return nullptr;
}

const ProgramText* Unit::getARandomGenerator() const
{
	if ( generators.size() > 0 )
		return generators[ qrand() % generators.size() ];
	return nullptr;
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
		bool stayInCurrentElement = false;
		while ( stayInCurrentElement || xmlReader.readNextStartElement() )
		{
			if ( ! loadDocumentChild(xmlReader, stayInCurrentElement)  )
				qCDebug(logApplication) << "Unit: ignoring element:" << xmlReader.name();
			stayInCurrentElement = false;
		}
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

bool Unit::loadDocumentChild(QXmlStreamReader& xmlReader, bool& stayInCurrentElement)
{
	// xmlREader has found a new element, load it according to its tag name
	if ( xmlReader.name() == "description" )
		descriptions.insert( xmlReader.attributes().value("lang").toString(), xmlReader.readElementText() );
	else if ( xmlReader.name() == "initial-code" )
		initialCodes.append( new ProgramText(ProgramText::initialCode, initialCodes.count() + 1, xmlReader)  );
	else if ( xmlReader.name() == "solution" )
		solutions.append( new ProgramText(ProgramText::solution, solutions.count() + 1, xmlReader)  );
	else if ( xmlReader.name() == "standard-generator" )
		generators.append( new ProgramText(ProgramText::standardGenerator, generators.count() + 1, xmlReader) );
	else if ( xmlReader.name() == "file-generator" )
		generators.append( new ProgramText(ProgramText::fileGenerator, generators.count() + 1, xmlReader)  );
	else if ( xmlReader.name() == "test-case" )
		loadTestCase(xmlReader, stayInCurrentElement);
	else
		return false;

	return true;
}

bool Unit::loadTestCase(QXmlStreamReader& xmlReader, bool& stayInCurrentElement)
{
	TestCase testCase;

	// Extract test-case arguments, if any
	testCase.args = xmlReader.attributes().value("args").toString();

	// An input element is mandatory
	if ( xmlReader.readNextStartElement() && xmlReader.name() == "input" )
		testCase.input = xmlReader.readElementText();
	else
		return false;

	// Followed by an output element
	if ( xmlReader.readNextStartElement() && xmlReader.name() == "output" )
		testCase.output = xmlReader.readElementText();
	else
		return false;

	// An standard error element is optional
	if ( xmlReader.readNextStartElement() && xmlReader.name() == "error" )
		testCase.error = xmlReader.readElementText();
	else
		stayInCurrentElement = true;

	testCases.append( testCase );
	return true;
}

bool Unit::loadTestCase(const QFileInfo& inputFileInfo)
{
	if ( ! inputFileInfo.isReadable() )
		return false;

	const QString& path = inputFileInfo.absolutePath();
	const QString& suffix = inputFileInfo.fileName().mid(5); // 5 = len("input")

	TestCase testCase;

	testCase.args   = Util::readAllText(path + "/args" + suffix);
	testCase.input  = Util::readAllText(inputFileInfo.absoluteFilePath());
	testCase.output = Util::readAllText(path + "/output" + suffix);
	testCase.error  = Util::readAllText(path + "/error" + suffix);

	testCases.append(testCase);
	return true;
}

bool Unit::loadDescription(const QFileInfo& problemFileInfo)
{
	if ( ! problemFileInfo.isReadable() )
		return false;

	// Get the language
	const QString& lang = problemFileInfo.fileName().mid(8); // 8 = len("problem.")
	descriptions.insert(lang, Util::readAllText(problemFileInfo.absoluteFilePath()));

	return true;
}

bool Unit::loadProgramText(const QFileInfo& fileInfo, QList<ProgramText*>& programTexts, ProgramText::Type type)
{
	if ( ! fileInfo.isReadable() )
		return false;

	const QString& code = Util::readAllText(fileInfo.absoluteFilePath());
	programTexts.append( new ProgramText(type, programTexts.count() + 1, fileInfo.suffix(), code) );

	return true;
}

bool Unit::validateUnit() const
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

// Used to generate filenames eg: "bn_gen_01.cpp"
static const char* programTextTypeStr[] =
{
	"unk",
	"ini",
	"gen",
	"gen",
	"sol",
};

QString ProgramText::buildBasename(bool appendExtension) const
{
	Q_ASSERT(id >= 0);
	const QString& basename = QString("bn_%1_%2").arg(programTextTypeStr[type]).arg(id, 2, 10, QLatin1Char('0'));
	if ( appendExtension )
		return basename + '.' + language;
	return basename;
}
