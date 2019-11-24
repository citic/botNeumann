#ifndef PLAYERSTATUS_H
#define PLAYERSTATUS_H

#include "LabelButton.h"

class Player;

/**
	@brief A visual control (label) that shows the nickname of the current player
	It emit pressed() signal when pressed
	It automatically updates when a new player is selected, because it autoconnects as lister of
	the PlayerManager object of the BotNeummanApp
 */
class PlayerStatus : public LabelButton
{
	Q_OBJECT
	Q_DISABLE_COPY(PlayerStatus)

  public:
	/// Constructor
	explicit PlayerStatus(QGraphicsItem* parentItem);
	/// Destructor
	virtual ~PlayerStatus() override;

  public slots:
	/// Emitted when the player has been changed for a new one
	void playerChanged(Player* newPlayer);
};

#endif // PLAYERSTATUS_H
