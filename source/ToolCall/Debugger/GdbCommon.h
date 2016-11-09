#ifndef GDBCOMMON_H
#define GDBCOMMON_H

/// The type of log message
enum GdbLogType
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_COMMAND_SENT,
	LOG_CONSOLE_OUTPUT
};

/// The state of GDB
enum GdbState
{
	STATE_STOPPED,
	STATE_RUNNING,
	STATE_FINISHED
};


#endif // GDBCOMMON_H
