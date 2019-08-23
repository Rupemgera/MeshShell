// CMakeProject1.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>

#include "widgets/mainwindow.h"
#include <QApplication>
#include <qfiledialog.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	//std::string filename = QFileDialog::getOpenFileName(0,"Read in Data files",DEFAULT_DATA_DIR).toStdString();
	//std::cout << filename << std::endl;

	return a.exec();
}
