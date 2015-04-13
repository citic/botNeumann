#ifndef TOOLCALL_H
#define TOOLCALL_H

#include <QObject>

/**
	@brief Base class to some build and debug tools: compiler, linker, debugger
 */
class ToolCall : public QObject
{
	Q_OBJECT

  public:
	/// Constructor
	explicit ToolCall(QObject* parent = nullptr);
	/// Destructor
	~ToolCall();
};

#endif // TOOLCALL_H
