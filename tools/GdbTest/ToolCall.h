#ifndef TOOLCALL_H
#define TOOLCALL_H

#include <QObject>
#include <QProcess>

class ToolCall : public QObject
{
	Q_OBJECT

  protected:
	QString toolName;
	QProcess process;

  public:
	explicit ToolCall(const QString& toolName, QObject *parent = nullptr);
	virtual ~ToolCall();

  signals:
	void userProgramFinished();

  public slots:
	void processUserProgramFinished();
	void processErrorOcurred(QProcess::ProcessError error);
};

#endif // TOOLCALL_H
