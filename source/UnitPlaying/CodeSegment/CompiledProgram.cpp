#include "CompiledProgram.h"
#include "Compiler.h"
#include "Diagnostic.h"
#include "LogManager.h"
#include "Unit.h"
#include "Util.h"
#include "PlayerSolution.h"

CompiledProgram::CompiledProgram(QObject* parent)
	: QObject(parent)
{
}

int CompiledProgram::getErrorCount() const
{
	Q_ASSERT(compiler);
	return compiler->getErrorCount();
}

bool CompiledProgram::build(const ProgramText* programText, const QString& destinationDirectory)
{
	// Store the program text for future use
	this->programText = programText;
	this->destinationDirectory = destinationDirectory;

	// Build the filenames: source(bn_gen_01.cpp) executable(bn_gen_01)
	const QString& sourcePath = destinationDirectory + '/' + programText->buildBasename(true);
	executablePath = destinationDirectory + '/' + programText->buildBasename(false);

	// Copy the source code of the generator to a file in player's solution
	ResourceToFileDumper dumper;
	if ( dumper.dumpString( programText->code, sourcePath ) == false )
		return false;

	// If there is a previous compile result, remove it
	if ( compiler )
		compiler->deleteLater();

	// Compile the generator's source code. It requires some time and we do not wait until it
	// finishes. When the compilation and linking process has finished, our compilerFinished()
	// is called and we continue processing the results there
	compiler = new Compiler(this);
	connect( compiler, SIGNAL(finished()), this, SIGNAL(buildFinished()) );
	compiler->compile(sourcePath, executablePath);

	return true;
}

bool CompiledProgram::logErrors() const
{
	qCCritical(logApplication) << "Generator failed to compile";
	const QList<Diagnostic*>& diagnostics = compiler->getAllDiagnostics();
	for ( int index = 0; index < diagnostics.count(); ++index )
		qCCritical(logApplication) << diagnostics[index]->buildUserText();
	return false;
}
