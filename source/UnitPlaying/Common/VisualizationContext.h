#ifndef VISUALIZATIONCONTEXT_H
#define VISUALIZATIONCONTEXT_H

enum VisualizationContext
{
	visUnknown,
	visStarting,
	visExecutionLoop,
	visStandardInput,
	visStandardOutput,
	visStandardError,
	visStopping,
	visMemoryMapper,
};

#define LOG_GDB_RESPONSES 0

#ifdef Q_OS_LINUX
const char* const nameForMalloc  = "__libc_malloc";
const char* const nameForCalloc  = "__libc_calloc";
const char* const nameForRealloc = "__libc_realloc";
const char* const nameForFree    = "__libc_free";

const char* const nameForStdinPtr  = "stdin->_IO_read_ptr";
const char* const nameForStdoutPtr = "stdout->_IO_write_ptr";
const char* const nameForStderrPtr = "stderr->_IO_write_ptr";
#else
const char* const nameForMalloc = "malloc";
const char* const nameForCalloc = "calloc";
const char* const nameForRealloc = "realloc";
const char* const nameForFree = "free";
#ifdef Q_OS_MAC
const char* const nameForStdinPtr  = "__stdinp";
const char* const nameForStdoutPtr = "__stdoutp";
const char* const nameForStderrPtr = "__stderrp";
#else
const char* const nameForStdinPtr  = "stdin";
const char* const nameForStdoutPtr = "stdout";
const char* const nameForStderrPtr = "stderr";
#endif
#endif

/// Macro to update the maxDuration parameter in virtual functions if the duration of the current
/// animation is longer than the maxDuration value currently known
#define updateMaxDuration(code) \
	{ \
		int duration = (code); \
		if ( duration > maxDuration ) \
			maxDuration = duration; \
	} \

#endif // VISUALIZATIONCONTEXT_H
