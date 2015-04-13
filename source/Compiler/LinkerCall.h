#ifndef LINKERCALL_H
#define LINKERCALL_H

#include "ToolCall.h"

/**
	@brief Calls the linker with a buch of object files to generate an executable
 */
class LinkerCall : public ToolCall
{
	Q_OBJECT

  public:
	/// Constructor
	explicit LinkerCall(QObject *parent = nullptr);
	/// Destructor
	~LinkerCall();
};

#endif // LINKERCALL_H
