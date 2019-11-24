#ifndef PLAYERMANAGERDIALOG_H
#define PLAYERMANAGERDIALOG_H

#include <QDialog>

class Player;
class PlayerManager;

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
	PlayerManager* playerManager;

  public:
	explicit PlayerManagerDialog(QWidget *parent = 0);
	virtual ~PlayerManagerDialog() override;

  protected slots:
	void createPlayerClicked();
	void removePlayerClicked();
	void renamePlayerClicked();
	void selectPlayerClicked();
	void nicknameLineEditChanged(QString text);
	void selectedPlayerChanged();

  protected:
	void loadPlayers();
	bool isUniqueNickname(const QString& text) const;
	void createItemForPlayer(const Player* player);
};

#endif // PLAYERMANAGERDIALOG_H
