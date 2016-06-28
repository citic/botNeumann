#ifndef USERPROGRAM_H
#define USERPROGRAM_H

#include <QObject>
#include <QProcess>

class UserProgram : public QObject
{
	Q_OBJECT

  protected:
	QProcess process;

  public:
	explicit UserProgram(QObject* parent = nullptr);
	virtual ~UserProgram();
	void start(const QString& executablePath);

  signals:
	void userProgramFinished();

  public slots:
	void processUserProgramFinished();
	void processErrorOcurred(QProcess::ProcessError error);
};

#endif // USERPROGRAM_H
