#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Source code is visible only in playing scenes
	ui->dockWidget->setVisible(false);

	ui->graphicsView->setScene(& scene);
}

MainWindow::~MainWindow()
{
	delete ui;
}
