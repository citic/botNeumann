#ifndef PLAYERMANAGERDIALOG_H
#define PLAYERMANAGERDIALOG_H

#include <QDialog>

namespace Ui { class PlayerManagerDialog; }

class PlayerManagerDialog : public QDialog
{
	Q_OBJECT

  protected:
	Ui::PlayerManagerDialog* ui;
	QPushButton* addPlayerButton;
	QPushButton* removePlayerButton;
	QPushButton* renamePlayerButton;
	QPushButton* changePlayerButton;

  public:
	explicit PlayerManagerDialog(QWidget *parent = 0);
	~PlayerManagerDialog();

  protected slots:
	void addPlayerClicked();
	void removePlayerClicked();
	void renamePlayerClicked();
	void changedPlayerClicked();
};

#endif // PLAYERMANAGERDIALOG_H
