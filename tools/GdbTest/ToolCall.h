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
	qint64 processId() const;

  signals:
	void toolFinished();
	void errorOccurred();

  public slots:
	void processFinished();
	void processErrorOccurred(QProcess::ProcessError error);
};

#endif // TOOLCALL_H
