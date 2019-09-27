#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "meshwidget.h"

#include <memory>

#include <QMainWindow>
#include <QFile>
#include <QWidget>
#include <QFileDialog>
#include <qdir.h>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	/*
	Properties
	*/

	/*UI*/

	Ui::MainWindow *ui;

	/*
	Widgets
	*/

	//MeshWidget *_mesh_widget;

	//vtkSmartPointer<vtkInteractorStyle> _style;

	/*
	Functions
	*/

	/*widget functions*/

	//void browserPrint(QString text);

	void addSlot();

	/*slots functions*/
private slots:

	void newMeshShell();

	void closeMeshShell(int id);

  void newEmptyMeshShell();

	// void trigerMemu(QAction *act);

	// void testButtonClicked();

	// void changeSettings();

	// void openMeshFile();
};

#endif // MAINWINDOW_H
