#include "InfoDialog.h"
#include "ui_InfoDialog.h"

InfoDialog::InfoDialog(const QString& title, const QString& htmlInfo, QWidget *parent)
	: QDialog(parent)
	, title(title)
	, ui(new Ui::InfoDialog)
{
	ui->setupUi(this);
	ui->infoTextEdit->setHtml(htmlInfo);
	setWindowTitle(title);
}

InfoDialog::~InfoDialog()
{
	delete ui;
}

bool InfoDialog::setInfo(const QString& title, const QString& htmlInfo, bool force)
{
	if ( ! force && this->title == title ) return false;
	this->title = title;
	setWindowTitle(this->title);
	ui->infoTextEdit->setHtml(htmlInfo);
	return true;
}
