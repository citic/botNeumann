#include "BotNeumannApp.h"
#include "PlayerManager.h"
#include "PlayerManagerDialog.h"
#include "ui_PlayerManagerDialog.h"
#include <QPushButton>

PlayerManagerDialog::PlayerManagerDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::PlayerManagerDialog)
{
	ui->setupUi(this);

	// Fill the list widget with the player list stored in this device
	PlayerManager* playerManager = BotNeumannApp::getInstance()->getPlayerManager();
	Q_ASSERT(playerManager);
	ui->playerListWidget->addItems(playerManager->fetchLocalPlayerNicknames());

	// Each time a nickname is typed in the nickname line edit
	connect(ui->nicknameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(nicknameLineEditChanged(QString)));

	// Each time a player is selected in the list
	connect(ui->playerListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectedPlayerChanged()));

	// Create a new player
	addPlayerButton = ui->buttonBox->addButton(tr("Add"), QDialogButtonBox::ActionRole);
	addPlayerButton->setEnabled(false);
	connect(addPlayerButton, SIGNAL(clicked()), this, SLOT(addPlayerClicked()));

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

void PlayerManagerDialog::addPlayerClicked()
{
	ui->playerListWidget->addItem(ui->nicknameLineEdit->text());
}

void PlayerManagerDialog::removePlayerClicked()
{
}

void PlayerManagerDialog::renamePlayerClicked()
{
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
	playerManager->setCurrentPlayer(selectedItem->text());
}

void PlayerManagerDialog::nicknameLineEditChanged(const QString& text)
{
	addPlayerButton->setEnabled( text.length() > 0 );
	renamePlayerButton->setEnabled( text.length() > 0 && ui->playerListWidget->selectedItems().count() > 0 );
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
