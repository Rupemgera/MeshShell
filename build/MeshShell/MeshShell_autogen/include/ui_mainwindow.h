/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpenMeshFile;
    QAction *actionOpenStressFile;
    QAction *action_vtk_settings;
    QAction *actionNewMeshShell;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QMenuBar *menuBar;
    QMenu *menuTest;
    QMenu *menufile;
    QMenu *menuVTK;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1000, 835);
        MainWindow->setMinimumSize(QSize(1000, 800));
        actionOpenMeshFile = new QAction(MainWindow);
        actionOpenMeshFile->setObjectName(QStringLiteral("actionOpenMeshFile"));
        actionOpenStressFile = new QAction(MainWindow);
        actionOpenStressFile->setObjectName(QStringLiteral("actionOpenStressFile"));
        action_vtk_settings = new QAction(MainWindow);
        action_vtk_settings->setObjectName(QStringLiteral("action_vtk_settings"));
        actionNewMeshShell = new QAction(MainWindow);
        actionNewMeshShell->setObjectName(QStringLiteral("actionNewMeshShell"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1000, 28));
        menuTest = new QMenu(menuBar);
        menuTest->setObjectName(QStringLiteral("menuTest"));
        menufile = new QMenu(menuBar);
        menufile->setObjectName(QStringLiteral("menufile"));
        menuVTK = new QMenu(menuBar);
        menuVTK->setObjectName(QStringLiteral("menuVTK"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuTest->menuAction());
        menuBar->addAction(menufile->menuAction());
        menuBar->addAction(menuVTK->menuAction());
        menuTest->addAction(actionNewMeshShell);
        menuVTK->addAction(action_vtk_settings);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "QVTK", Q_NULLPTR));
        actionOpenMeshFile->setText(QApplication::translate("MainWindow", "Open Mesh File", Q_NULLPTR));
        actionOpenStressFile->setText(QApplication::translate("MainWindow", "Open Stress File", Q_NULLPTR));
        action_vtk_settings->setText(QApplication::translate("MainWindow", "vtk settings", Q_NULLPTR));
        actionNewMeshShell->setText(QApplication::translate("MainWindow", "MeshShell", Q_NULLPTR));
        menuTest->setTitle(QApplication::translate("MainWindow", "New", Q_NULLPTR));
        menufile->setTitle(QApplication::translate("MainWindow", "file", Q_NULLPTR));
        menuVTK->setTitle(QApplication::translate("MainWindow", "Settings", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
