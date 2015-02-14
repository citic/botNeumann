#ifndef CODEEDITORDOCKWIDGET_H
#define CODEEDITORDOCKWIDGET_H

#include <QDockWidget>

class CodeEditorDockWidget : public QDockWidget
{
	Q_OBJECT

  public:
	/// Constructor
	explicit CodeEditorDockWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~CodeEditorDockWidget();

};

#endif // CODEEDITORDOCKWIDGET_H
