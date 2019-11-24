#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY(InfoDialog)

  protected:
	/// The title and identifier of the information
	QString title;
	/// Designed with QtCreator
	Ui::InfoDialog *ui;

  public:
	/// Constructor
	explicit InfoDialog(const QString& title, const QString& htmlInfo, QWidget *parent = nullptr);
	/// Destructor
	virtual ~InfoDialog() override;
	/// Shows the given information in the dialog
	/// @param force If false and the information is already shown, it will not rendered again
	/// (cached). Send true to overwrite it, or send a different title
	/// @return true if the information was changed, false if there was no need to do it
	bool setInfo(const QString& title, const QString& htmlInfo, bool force = false);
};

#endif // INFODIALOG_H
