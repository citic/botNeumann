#ifndef BOTNEUMANNAPP_H
#define BOTNEUMANNAPP_H

#include <QApplication>

/** Object that represents the bot Neumann application as a whole */
class BotNeumannApp : public QApplication
{
	Q_OBJECT

  public:
	/// Constructor
	explicit BotNeumannApp(int argc, char *argv[]);

  signals:

  public slots:

};

#endif // BOTNEUMANNAPP_H
