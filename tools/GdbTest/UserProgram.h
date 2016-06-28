#ifndef USERPROGRAM_H
#define USERPROGRAM_H

#include "ToolCall.h"

class UserProgram : public ToolCall
{
	Q_OBJECT

  public:
	explicit UserProgram(QObject* parent = nullptr);
	virtual ~UserProgram();
	void start(const QString& executablePath);
};

#endif // USERPROGRAM_H
