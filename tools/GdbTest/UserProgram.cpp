#include "UserProgram.h"

UserProgram::UserProgram(QObject* parent)
	: ToolCall("UserProgram", parent)
{
}

UserProgram::~UserProgram()
{
}

void UserProgram::start(const QString& executablePath)
{
	printf("%s: starting %s\n", qPrintable(toolName), qPrintable(executablePath));
	process.setProgram(executablePath);
	//ToDo: process.setArguments(arguments);
	//Old: process.start(executablePath);
	process.open();
	printf("%s: process id %lli\n", qPrintable(toolName), process.processId());
}
