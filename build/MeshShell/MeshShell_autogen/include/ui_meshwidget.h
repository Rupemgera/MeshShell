/********************************************************************************
** Form generated from reading UI file 'meshwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESHWIDGET_H
#define UI_MESHWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "QVTKOpenGLWidget.h"

QT_BEGIN_NAMESPACE

class Ui_MeshWidget
{
public:
    QGridLayout *gridLayout_2;
    QVTKOpenGLWidget *viewerWidget;
    QTabWidget *tabWidget;
    QWidget *tab_mesh;
    QGridLayout *gridLayout;
    QGroupBox *groupBox_geometry;
    QGridLayout *gridLayout_3;
    QCheckBox *checkBox_edge;
    QCheckBox *checkBox_face;
    QPushButton *pushButton_read;
    QGroupBox *groupBox_color;
    QGridLayout *gridLayout_6;
    QLabel *label_edges_count;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_faces_count;
    QLabel *label_cells_count;
    QLabel *label_4;
    QLabel *label_vertices_count;
    QWidget *tab_stress_field;
    QGridLayout *gridLayout_4;
    QPushButton *pushButton_readStressFile;
    QGroupBox *groupBox_render_stress;
    QGridLayout *gridLayout_5;
    QCheckBox *checkBox_mid;
    QCheckBox *checkBox_major;
    QCheckBox *checkBox_minor;

    void setupUi(QWidget *MeshWidget)
    {
        if (MeshWidget->objectName().isEmpty())
            MeshWidget->setObjectName(QStringLiteral("MeshWidget"));
        MeshWidget->resize(1027, 873);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MeshWidget->sizePolicy().hasHeightForWidth());
        MeshWidget->setSizePolicy(sizePolicy);
        MeshWidget->setMinimumSize(QSize(150, 0));
        gridLayout_2 = new QGridLayout(MeshWidget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        viewerWidget = new QVTKOpenGLWidget(MeshWidget);
        viewerWidget->setObjectName(QStringLiteral("viewerWidget"));
        sizePolicy.setHeightForWidth(viewerWidget->sizePolicy().hasHeightForWidth());
        viewerWidget->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(viewerWidget, 0, 0, 1, 1);

        tabWidget = new QTabWidget(MeshWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabWidget->setMinimumSize(QSize(0, 150));
        tab_mesh = new QWidget();
        tab_mesh->setObjectName(QStringLiteral("tab_mesh"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tab_mesh->sizePolicy().hasHeightForWidth());
        tab_mesh->setSizePolicy(sizePolicy2);
        tab_mesh->setMinimumSize(QSize(0, 0));
        gridLayout = new QGridLayout(tab_mesh);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox_geometry = new QGroupBox(tab_mesh);
        groupBox_geometry->setObjectName(QStringLiteral("groupBox_geometry"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox_geometry->sizePolicy().hasHeightForWidth());
        groupBox_geometry->setSizePolicy(sizePolicy3);
        gridLayout_3 = new QGridLayout(groupBox_geometry);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        checkBox_edge = new QCheckBox(groupBox_geometry);
        checkBox_edge->setObjectName(QStringLiteral("checkBox_edge"));
        checkBox_edge->setChecked(true);

        gridLayout_3->addWidget(checkBox_edge, 1, 0, 1, 1);

        checkBox_face = new QCheckBox(groupBox_geometry);
        checkBox_face->setObjectName(QStringLiteral("checkBox_face"));
        checkBox_face->setChecked(true);

        gridLayout_3->addWidget(checkBox_face, 2, 0, 1, 1);


        gridLayout->addWidget(groupBox_geometry, 1, 0, 1, 1);

        pushButton_read = new QPushButton(tab_mesh);
        pushButton_read->setObjectName(QStringLiteral("pushButton_read"));

        gridLayout->addWidget(pushButton_read, 0, 0, 1, 1);

        groupBox_color = new QGroupBox(tab_mesh);
        groupBox_color->setObjectName(QStringLiteral("groupBox_color"));
        gridLayout_6 = new QGridLayout(groupBox_color);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label_edges_count = new QLabel(groupBox_color);
        label_edges_count->setObjectName(QStringLiteral("label_edges_count"));

        gridLayout_6->addWidget(label_edges_count, 1, 1, 1, 1);

        label = new QLabel(groupBox_color);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_6->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(groupBox_color);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_6->addWidget(label_2, 2, 0, 1, 1);

        label_3 = new QLabel(groupBox_color);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_6->addWidget(label_3, 3, 0, 1, 1);

        label_faces_count = new QLabel(groupBox_color);
        label_faces_count->setObjectName(QStringLiteral("label_faces_count"));

        gridLayout_6->addWidget(label_faces_count, 2, 1, 1, 1);

        label_cells_count = new QLabel(groupBox_color);
        label_cells_count->setObjectName(QStringLiteral("label_cells_count"));

        gridLayout_6->addWidget(label_cells_count, 3, 1, 1, 1);

        label_4 = new QLabel(groupBox_color);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_6->addWidget(label_4, 0, 0, 1, 1);

        label_vertices_count = new QLabel(groupBox_color);
        label_vertices_count->setObjectName(QStringLiteral("label_vertices_count"));

        gridLayout_6->addWidget(label_vertices_count, 0, 1, 1, 1);


        gridLayout->addWidget(groupBox_color, 1, 1, 1, 1);

        tabWidget->addTab(tab_mesh, QString());
        tab_stress_field = new QWidget();
        tab_stress_field->setObjectName(QStringLiteral("tab_stress_field"));
        gridLayout_4 = new QGridLayout(tab_stress_field);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        pushButton_readStressFile = new QPushButton(tab_stress_field);
        pushButton_readStressFile->setObjectName(QStringLiteral("pushButton_readStressFile"));

        gridLayout_4->addWidget(pushButton_readStressFile, 0, 0, 1, 1);

        groupBox_render_stress = new QGroupBox(tab_stress_field);
        groupBox_render_stress->setObjectName(QStringLiteral("groupBox_render_stress"));
        gridLayout_5 = new QGridLayout(groupBox_render_stress);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        checkBox_mid = new QCheckBox(groupBox_render_stress);
        checkBox_mid->setObjectName(QStringLiteral("checkBox_mid"));

        gridLayout_5->addWidget(checkBox_mid, 0, 1, 1, 1);

        checkBox_major = new QCheckBox(groupBox_render_stress);
        checkBox_major->setObjectName(QStringLiteral("checkBox_major"));
        checkBox_major->setChecked(true);

        gridLayout_5->addWidget(checkBox_major, 0, 0, 1, 1);

        checkBox_minor = new QCheckBox(groupBox_render_stress);
        checkBox_minor->setObjectName(QStringLiteral("checkBox_minor"));

        gridLayout_5->addWidget(checkBox_minor, 0, 2, 1, 1);


        gridLayout_4->addWidget(groupBox_render_stress, 1, 0, 1, 1);

        tabWidget->addTab(tab_stress_field, QString());

        gridLayout_2->addWidget(tabWidget, 1, 0, 1, 1);


        retranslateUi(MeshWidget);
        QObject::connect(checkBox_mid, SIGNAL(toggled(bool)), MeshWidget, SLOT(renderField()));
        QObject::connect(pushButton_readStressFile, SIGNAL(clicked()), MeshWidget, SLOT(openStressFile()));
        QObject::connect(checkBox_minor, SIGNAL(toggled(bool)), MeshWidget, SLOT(renderField()));
        QObject::connect(checkBox_face, SIGNAL(toggled(bool)), MeshWidget, SLOT(updateMesh()));
        QObject::connect(checkBox_edge, SIGNAL(toggled(bool)), MeshWidget, SLOT(updateMesh()));
        QObject::connect(checkBox_major, SIGNAL(toggled(bool)), MeshWidget, SLOT(renderField()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MeshWidget);
    } // setupUi

    void retranslateUi(QWidget *MeshWidget)
    {
        MeshWidget->setWindowTitle(QApplication::translate("MeshWidget", "Form", Q_NULLPTR));
        groupBox_geometry->setTitle(QApplication::translate("MeshWidget", "Geometry", Q_NULLPTR));
        checkBox_edge->setText(QApplication::translate("MeshWidget", "edge", Q_NULLPTR));
        checkBox_face->setText(QApplication::translate("MeshWidget", "face", Q_NULLPTR));
        pushButton_read->setText(QApplication::translate("MeshWidget", "read", Q_NULLPTR));
        groupBox_color->setTitle(QApplication::translate("MeshWidget", "Info", Q_NULLPTR));
        label_edges_count->setText(QString());
        label->setText(QApplication::translate("MeshWidget", "edges:", Q_NULLPTR));
        label_2->setText(QApplication::translate("MeshWidget", "faces", Q_NULLPTR));
        label_3->setText(QApplication::translate("MeshWidget", "cells", Q_NULLPTR));
        label_faces_count->setText(QString());
        label_cells_count->setText(QString());
        label_4->setText(QApplication::translate("MeshWidget", "vertices", Q_NULLPTR));
        label_vertices_count->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_mesh), QApplication::translate("MeshWidget", "Mesh", Q_NULLPTR));
        pushButton_readStressFile->setText(QApplication::translate("MeshWidget", "Read Stress", Q_NULLPTR));
        groupBox_render_stress->setTitle(QApplication::translate("MeshWidget", "Render Stress", Q_NULLPTR));
        checkBox_mid->setText(QApplication::translate("MeshWidget", "mid", Q_NULLPTR));
        checkBox_major->setText(QApplication::translate("MeshWidget", "major", Q_NULLPTR));
        checkBox_minor->setText(QApplication::translate("MeshWidget", "minor", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_stress_field), QApplication::translate("MeshWidget", "Stress Field", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MeshWidget: public Ui_MeshWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHWIDGET_H
