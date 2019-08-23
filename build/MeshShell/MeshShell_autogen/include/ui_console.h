/********************************************************************************
** Form generated from reading UI file 'console.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSOLE_H
#define UI_CONSOLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConsoleWidget
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *ConsoleWidget)
    {
        if (ConsoleWidget->objectName().isEmpty())
            ConsoleWidget->setObjectName(QStringLiteral("ConsoleWidget"));
        ConsoleWidget->resize(400, 321);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ConsoleWidget->sizePolicy().hasHeightForWidth());
        ConsoleWidget->setSizePolicy(sizePolicy);
        ConsoleWidget->setMinimumSize(QSize(0, 150));
        gridLayout = new QGridLayout(ConsoleWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        textBrowser = new QTextBrowser(ConsoleWidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        verticalLayout->addWidget(textBrowser);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(ConsoleWidget);

        QMetaObject::connectSlotsByName(ConsoleWidget);
    } // setupUi

    void retranslateUi(QWidget *ConsoleWidget)
    {
        ConsoleWidget->setWindowTitle(QApplication::translate("ConsoleWidget", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ConsoleWidget: public Ui_ConsoleWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSOLE_H
