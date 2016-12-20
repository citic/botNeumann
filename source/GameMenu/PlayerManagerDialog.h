#ifndef PLAYERMANAGERDIALOG_H
#define PLAYERMANAGERDIALOG_H

#include <QDialog>

namespace Ui { class PlayerManagerDialog; }

class PlayerManagerDialog : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY(PlayerManagerDialog)

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
	void selectPlayerClicked();
	void nicknameLineEditChanged(const QString& text);
	void selectedPlayerChanged();
};

#endif // PLAYERMANAGERDIALOG_H
