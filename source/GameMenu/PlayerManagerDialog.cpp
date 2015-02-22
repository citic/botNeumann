#include "PlayerManagerDialog.h"
#include "ui_PlayerManagerDialog.h"
#include <QPushButton>

PlayerManagerDialog::PlayerManagerDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::PlayerManagerDialog)
{
	ui->setupUi(this);

	// Create a new player
	addPlayerButton = ui->buttonBox->addButton(tr("Add"), QDialogButtonBox::ActionRole);
	connect(addPlayerButton, SIGNAL(clicked()), this, SLOT(addPlayerClicked()));

	// Remove the selected  player
	removePlayerButton = ui->buttonBox->addButton(tr("Remove"), QDialogButtonBox::ActionRole);
	connect(removePlayerButton, SIGNAL(clicked()), this, SLOT(removePlayerClicked()));

	// Rename the selected  player
	renamePlayerButton = ui->buttonBox->addButton(tr("Rename"), QDialogButtonBox::ActionRole);
	connect(renamePlayerButton, SIGNAL(clicked()), this, SLOT(renamePlayerClicked()));

	// Set the new current player
	changePlayerButton = ui->buttonBox->addButton(tr("Change"), QDialogButtonBox::AcceptRole);
	connect(changePlayerButton, SIGNAL(clicked()), this, SLOT(changedPlayerClicked()));
}

PlayerManagerDialog::~PlayerManagerDialog()
{
	delete ui;
}

void PlayerManagerDialog::addPlayerClicked()
{

}

void PlayerManagerDialog::removePlayerClicked()
{

}

void PlayerManagerDialog::renamePlayerClicked()
{

}

void PlayerManagerDialog::changedPlayerClicked()
{

}
