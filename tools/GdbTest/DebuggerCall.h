#ifndef DEBUGGERCALL_H
#define DEBUGGERCALL_H

#include <QObject>
#include <QProcess>

class DebuggerCall : public QObject
{
	Q_OBJECT

  protected:
	QProcess process;

  public:
	explicit DebuggerCall(QObject *parent = nullptr);
	virtual ~DebuggerCall();
	bool start();

  signals:

  public slots:
};

#endif // DEBUGGERCALL_H
