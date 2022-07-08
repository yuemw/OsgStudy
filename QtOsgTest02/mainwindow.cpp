#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "osg/Osg3dView.h"

#include <QSettings>
#include <QFileDialog>

#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	ui->osg3dview->setScene(&m_itemModel);

//	connect(ui->actionopen, &QAction::triggered, this, &MainWindow::on_actionFileOpen_triggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionopen_triggered()
{
	QSettings settings;
	settings.value("currentDirectory");
	QString fileName = QFileDialog::getOpenFileName(this,
		"Select File",
		settings.value("currentDirectory").toString(),
		"OpenSceneGraph (*.osg *.ive *.osgt *.osgb *.obj *earth)");
	if (fileName.isEmpty())
		return;

	settings.setValue("currentDirectory", QVariant(QFileInfo(fileName).path()));

	m_itemModel.importFileByName(fileName);
	settings.setValue("recentFile", fileName);
}