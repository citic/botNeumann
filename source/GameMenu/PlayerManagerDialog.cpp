#include "BotNeumannApp.h"
#include "Player.h"
#include "PlayerManager.h"
#include "PlayerManagerDialog.h"
#include "ui_PlayerManagerDialog.h"
#include <QMessageBox>
#include <QPushButton>

PlayerManagerDialog::PlayerManagerDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::PlayerManagerDialog)
{
	ui->setupUi(this);

	loadPlayers();

	// Each time a nickname is typed in the nickname line edit
	connect(ui->nicknameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(nicknameLineEditChanged(QString)));

	// Each time a player is selected in the list
	connect(ui->playerListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectedPlayerChanged()));

	// Create a new player
	addPlayerButton = ui->buttonBox->addButton(tr("Create"), QDialogButtonBox::ActionRole);
	addPlayerButton->setEnabled(false);
	connect(addPlayerButton, SIGNAL(clicked()), this, SLOT(createPlayerClicked()));

	// Remove the selected  player
	removePlayerButton = ui->buttonBox->addButton(tr("Remove"), QDialogButtonBox::ActionRole);
	removePlayerButton->setEnabled(false);
	connect(removePlayerButton, SIGNAL(clicked()), this, SLOT(removePlayerClicked()));

	// Rename the selected  player
	renamePlayerButton = ui->buttonBox->addButton(tr("Rename"), QDialogButtonBox::ActionRole);
	renamePlayerButton->setEnabled(false);
	connect(renamePlayerButton, SIGNAL(clicked()), this, SLOT(renamePlayerClicked()));

	// Set the new current player
	changePlayerButton = ui->buttonBox->addButton(tr("Select"), QDialogButtonBox::AcceptRole);
	changePlayerButton->setEnabled(false);
	connect(changePlayerButton, SIGNAL(clicked()), this, SLOT(selectPlayerClicked()));
}

PlayerManagerDialog::~PlayerManagerDialog()
{
	delete ui;
}

void PlayerManagerDialog::loadPlayers()
{
	// Fill the list widget with the player list stored in this device
	PlayerManager* playerManager = BotNeumannApp::getInstance()->getPlayerManager();
	Q_ASSERT(playerManager);

	// Load players from settings and get the number of loaded players
	int playerCount = playerManager->loadPlayers();

	// Fill the list with players' nicknames and IDs
	for ( int playerIndex = 0; playerIndex < playerCount; ++playerIndex )
		createItemForPlayer( playerManager->getPlayerAt(playerIndex) );
}

void PlayerManagerDialog::createPlayerClicked()
{
	const QString& nickname = ui->nicknameLineEdit->text().trimmed();

	PlayerManager* playerManager = BotNeumannApp::getInstance()->getPlayerManager();
	Q_ASSERT(playerManager);
	Player* newPlayer = playerManager->createPlayer(nickname);
	if ( newPlayer )
	{
		createItemForPlayer(newPlayer);
		emit accept();
	}
	else
		QMessageBox::critical(this, tr("Error"), tr("Could not create player: ") + nickname);
}

void PlayerManagerDialog::createItemForPlayer(const Player* player)
{
	QListWidgetItem* item = new QListWidgetItem(player->getNickname(), ui->playerListWidget );
	item->setData( Qt::UserRole, player->getId() );
}

void PlayerManagerDialog::removePlayerClicked()
{
}

void PlayerManagerDialog::renamePlayerClicked()
{
	const QString& newNickname = ui->nicknameLineEdit->text().trimmed();

	if ( ui->playerListWidget->selectedItems().size() < 0 ) return;
	QListWidgetItem* selectedItem = ui->playerListWidget->selectedItems()[0];
	Q_ASSERT(selectedItem);

	// Tell to the player manager who is the current active player
	PlayerManager* playerManager = BotNeumannApp::getInstance()->getPlayerManager();
	Q_ASSERT(playerManager);
	const QByteArray& playerId = selectedItem->data(Qt::UserRole).toByteArray();
	const QString& oldNickname = selectedItem->text();
	if ( playerManager->renamePlayer( playerId, newNickname ) )
		selectedItem->setText(newNickname);
	else
		QMessageBox::critical(this, tr("Error"), tr("Could not rename player %1 to %2").arg(oldNickname).arg(newNickname));
}

void PlayerManagerDialog::selectPlayerClicked()
{
	// Get the selected player in the list
	if ( ui->playerListWidget->selectedItems().size() < 0 ) return;
	QListWidgetItem* selectedItem = ui->playerListWidget->selectedItems()[0];
	Q_ASSERT(selectedItem);

	// Tell to the player manager who is the current active player
	PlayerManager* playerManager = BotNeumannApp::getInstance()->getPlayerManager();
	Q_ASSERT(playerManager);
	playerManager->setCurrentPlayer( selectedItem->data(Qt::UserRole).toByteArray() );
}

void PlayerManagerDialog::nicknameLineEditChanged(QString text)
{
	text = text.trimmed();
	addPlayerButton->setEnabled( text.length() > 0 && isUniqueNickname(text) );
	renamePlayerButton->setEnabled( text.length() > 0 && ui->playerListWidget->selectedItems().count() > 0 && isUniqueNickname(text) );
}

bool PlayerManagerDialog::isUniqueNickname(const QString& text) const
{
	for ( int index = 0; index < ui->playerListWidget->count(); ++index )
		if ( ui->playerListWidget->item(index)->text() == text )
			return false;

	return true;
}

void PlayerManagerDialog::selectedPlayerChanged()
{
	bool isThereSelection = ui->playerListWidget->selectedItems().size() >= 0;
	changePlayerButton->setEnabled(isThereSelection);
	removePlayerButton->setEnabled(isThereSelection);
	if ( ! isThereSelection ) return;
	QListWidgetItem* selectedItem = ui->playerListWidget->selectedItems()[0];
	ui->nicknameLineEdit->setText( selectedItem->text() );
}
