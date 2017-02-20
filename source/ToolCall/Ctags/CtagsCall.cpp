#include "CtagsCall.h"
#include "Common.h"
#include "LogManager.h"

#include <QFileInfoList>
#include <QRegularExpression>

#ifdef Q_OS_MAC
	static const QString ctagsPath = QStringLiteral("/opt/local/bin/ctags");
#else
	static const QString ctagsPath = QStringLiteral("ctags");
#endif

bool Symbol::loadFrom(const QString& line)
{
	// We have two types of lines in bn_tags.txt:

	// func_name\tfile.cpp\t/^\tfull decl $/;"\tfunction\tline:##\tdetails\0
	// gvar_name\tfile.cpp\t/^full_decl_ws$/;"\tvariable\tline:##\0

	// Fields cannot be obtained by line.split('\t') because the full declaration can contain tabs
	// if the declaration has them in the source file. We use a regular expression:

	// ^(.+)\t(.+)\t/\^(.+)\$/;"\t(.+)\tline:(\d+)\t?(.*)$
	//   1     2        3          4          5       6
	//   name  file     decl       type       line    details

	// We have to escape backslashes and quotes in C++
	QRegularExpression re("^(.+)\\t(.+)\\t/\\^(.+)\\$/;\"\\t(.+)\\tline:(\\d+)\\t?(.*)$");

	// If the line matches our regular expression, it has a symbol
	QRegularExpressionMatch match = re.match(line);
	if ( match.hasMatch() == false )
		return false;

	// Extract each field using the groups in regular expression
	name = match.captured(1);
	filename = match.captured(2);
	declaration = match.captured(3);
	type = mapTextToType( match.captured(4) );
	this->line = match.captured(5).toLongLong();
	details = match.captured(6);

	// ToDo: Return if this symbol is valid
	// return isValid();
	return true;
}

Symbol* Symbol::createFrom(const QString& line)
{
	// Create a Symbol object
	Symbol* symbol = new Symbol();

	// If it loaded successfully from the line, return it
	if ( symbol->loadFrom(line) )
		return symbol;

	// The load failed
	delete symbol;
	return nullptr;
}

Symbol::Type Symbol::mapTextToType(const QString& text)
{
	if ( text == "function" ) return functionDefinition;
	if ( text == "variable" ) return globalVariable;
	return unknown;
}


CtagsCall::CtagsCall(QObject* parent)
	: ToolCall(parent)
{
}

CtagsCall::~CtagsCall()
{
	DELETE_POINTERS_ARRAY(allSymbols);
}

bool CtagsCall::extractSymbols(const QFileInfoList& sourceFiles, const QString& outputDirectory)
{
	// Build the arguments to call ctags command
	// Eg: ctags --c++-kinds=vf --fields=fKsnt --if0=yes --sort=no -f bn_tags.txt alli.cpp
	arguments.clear();
	arguments << "--c++-kinds=vf"; // Extract global variables and function definitions
	arguments << "--fields=fKsnt"; // Extract the symbol name, file, line number
	arguments << "--if0=yes"; // Ignore code inside #if 0 ... #endif
	arguments << "--sort=no"; // Do not sort symbols, keep their order in files

	// Build the ctags output filepath
	ctagsFilepath = outputDirectory + "/bn_tags.txt";
	arguments << "-f" << ctagsFilepath;

	// Add all the source files to the argument list
	Q_ASSERT(sourceFiles.count() > 0);
	for ( int index = 0; index < sourceFiles.count(); ++index )
		arguments.append( sourceFiles[index].absoluteFilePath() );

	// Create a process an when it finishes, we continue with the symbol extraction
	process = new QProcess(this);
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished()));
	connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processFailed()));

	// Start the process
	process->start(ctagsPath, arguments);
	qCInfo(logBuild).noquote() << process->program() << process->arguments().join(" ");
	state = ToolCallState::started;
	return true;
}

bool CtagsCall::processFinished()
{
	qCInfo(logBuild) << "Ctags finished with exit code" << process->exitCode() << "and exit status" << process->exitStatus();

	// The generation of bn_tags.txt finished, open this file
	QFile inputFile(ctagsFilepath);
	if ( inputFile.open(QIODevice::ReadOnly | QIODevice::Text) == false )
	{
		// Log an error and return an invalid file path
		qCritical(logApplication) << "Could not open" << ctagsFilepath;
		return false;
	}

	// The bn_tags.txt is a text file, it contains the symbols extracted
	QTextStream inputText(&inputFile);
	while ( ! inputText.atEnd() )
		if ( ! extractSymbolFrom( inputText.readLine() ) )
			return false;

	// Alert others the process has finished
	emit extractionFinished();

	// Destructors will close the files
	return true;
}

bool CtagsCall::extractSymbolFrom(const QString& line)
{
	// Ignore empty and comment lines
	if ( line.isEmpty() || line[0] == '!' )
		return true;

	// The line contains a symbol, convert it to an object
	Symbol* symbol = Symbol::createFrom(line);
	if ( symbol == nullptr )
		return false;

	// Add the symbol to the respective list
	allSymbols.append(symbol);

	// If the symbol is a global variable, include it also in that list
	if ( symbol->type == Symbol::globalVariable )
		globalVariables.append( symbol );

	// If the symbol is a function definition, include it also in that list
	if ( symbol->type == Symbol::functionDefinition )
		functionDefinitions.append( symbol );

	// Done
	return true;
}

void CtagsCall::processFailed()
{
	qCInfo(logBuild) << "Ctags failed with exit code" << process->exitCode() << "and exit status" << process->exitStatus();
	emit extractionFailed( process->error() );
}
