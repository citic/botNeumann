#include "UserProgram.h"
#include <stdio.h>

UserProgram::UserProgram(QObject* parent)
	: QObject(parent)
	, process(this)
{
}

UserProgram::~UserProgram()
{
	fprintf(stderr, "UserProgram object deleted\n");
}

void UserProgram::start(const QString& executablePath)
{
	connect( &process, SIGNAL(finished(int)), this, SLOT(processUserProgramFinished()) );
	process.setProgram(executablePath);
	//ToDo: process.setArguments(arguments);
	//Old: process.start(executablePath);
	printf("UserProgram: process id: %lli\n", process.processId() );
	process.open();
	printf("UserProgram: process id: %lli\n", process.processId() );
}

void UserProgram::processUserProgramFinished()
{
	fprintf(stderr, "UserProgram execution finished\n");
	emit userProgramFinished();
}

void UserProgram::processErrorOcurred(QProcess::ProcessError error)
{
	fprintf(stderr, "UserProgram error ocurred: %i\n", (int)error);
}
