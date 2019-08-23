#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	addSlot();
}

MainWindow::~MainWindow()
{
	delete ui;
}

// void MainWindow::browserPrint(QString text)
// {
// 	//this->ui->textBrowser->append(text + "\n");

// 	//ConsoleWidget* con =
// 	_console_widget->print(text);
// }

void MainWindow::addSlot()
{
	// signal slot

	connect(this->ui->actionNewMeshShell, &QAction::triggered, this, &MainWindow::NewMeshShell);

	//测试按钮
	//connect(this->ui->testButton, &QPushButton::clicked, this, &MainWindow::testButtonClicked);

	//打开mesh文件
	//connect(this->ui->actionOpenMeshFile, &QAction::triggered, this, &MainWindow::openMeshFile);

	//打开stress field 文件
	//connect(this->ui->actionOpenStressFile, &QAction::triggered, this, &MainWindow::openStressFile);

	// render mesh
	//connect(this->ui->pushButton_draw_mesh, &QPushButton::clicked, this, &MainWindow::drawMesh);
}

void MainWindow::NewMeshShell()
{
	auto mesh_widget = new MeshWidget(this);
	ui->tabWidget->addTab(mesh_widget, "Mesh Shell");
	ui->tabWidget->setCurrentWidget(mesh_widget);
	ui->tabWidget->show();
}

// void MainWindow::trigerMemu(QAction *act)
// {
// 	if (act->text() == "OpenMeshFile")
// 	{
// 		openMeshFile();
// 	}
// }

// void MainWindow::testButtonClicked()
// {
// 	browserPrint("test button clicked");
// 	//_vtk_mesh->testVTKWrapper();
// }

// void MainWindow::openMeshFile()
// {
// 	//
// 	browserPrint("start open mesh file");

// 	QString qfname = QFileDialog::getOpenFileName(0, "Open mesh file",
// 												  QDir::currentPath(), "OVM files(*.ovm);;Abaqus inp files(*.inp)");

// 	if (qfname == "")
// 	{
// 		browserPrint("cancel open mesh file");
// 		return;
// 	}

// 	std::string filename = qfname.toStdString();

// 	size_t dot_position = filename.find_last_of('.');

// #ifdef WIN32
// 	size_t slash_position = filename.find_last_of('/');
// #endif // WIN32

// #ifdef __linux
// 	size_t slash_position = filename.find_last_of('/');
// #endif // __linux

// 	if (slash_position == filename.npos)
// 	{
// 		slash_position = 0;
// 	}

// 	std::string name_without_path = filename.substr(slash_position + 1);
// 	std::string extension = filename.substr(dot_position + 1);
// 	std::string rest_filename = filename.substr(0, dot_position + 1);

// 	std::ifstream fin(filename);
// 	std::shared_ptr<VMesh> mesh_tmp;
// 	if (extension == "ovm")
// 	{
// 		mesh_tmp = meshtools::MeshProcessingTools::readFromOvm(fin);
// 	}
// 	else
// 	{
// 		mesh_tmp = meshtools::MeshProcessingTools::readFromInp(fin);
// 		// if read in a inp file, then save mesh to ovm file for future use
// 		meshtools::MeshProcessingTools::saveToOVM(mesh_tmp, rest_filename + "ovm");
// 	}
// 	fin.close();

// 	//_vtk_mesh->set_mesh(mesh_tmp);

// 	// set _mesh_loaded true
// 	//_mesh_loaded = true;

// 	//auto draw mesh
// 	//drawMesh();

// 	auto mesh_widget = new MeshWidget(mesh_tmp, ui->vtkWidget, this);
// 	ui->tabWidget->addTab(mesh_widget, QString(name_without_path.c_str()));
// 	ui->tabWidget->setCurrentWidget(mesh_widget);
// 	ui->tabWidget->show();
// 	_mesh_widget = (MeshWidget *)ui->tabWidget->currentWidget();

// 	_mesh_widget->set_console(_console_widget);

// 	browserPrint(qfname);

// 	browserPrint("open mesh file done");
// }

// void MainWindow::changeSettings()
// {
// }
