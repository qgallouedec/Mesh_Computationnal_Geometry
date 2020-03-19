/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "gldisplaywidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    GLDisplayWidget *widget;
    QPushButton *pushButton;
    QCheckBox *checkBox_Vertices;
    QCheckBox *checkBox_Edges;
    QCheckBox *checkBox_Faces;
    QSlider *horizontalSlider;
    QCheckBox *checkBox_Faces_2;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(747, 528);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widget = new GLDisplayWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(140, 10, 601, 491));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(20, 370, 112, 32));
        checkBox_Vertices = new QCheckBox(centralWidget);
        checkBox_Vertices->setObjectName(QStringLiteral("checkBox_Vertices"));
        checkBox_Vertices->setGeometry(QRect(20, 100, 86, 20));
        checkBox_Edges = new QCheckBox(centralWidget);
        checkBox_Edges->setObjectName(QStringLiteral("checkBox_Edges"));
        checkBox_Edges->setGeometry(QRect(20, 130, 86, 20));
        checkBox_Faces = new QCheckBox(centralWidget);
        checkBox_Faces->setObjectName(QStringLiteral("checkBox_Faces"));
        checkBox_Faces->setGeometry(QRect(20, 160, 86, 20));
        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(10, 240, 111, 22));
        horizontalSlider->setOrientation(Qt::Horizontal);
        checkBox_Faces_2 = new QCheckBox(centralWidget);
        checkBox_Faces_2->setObjectName(QStringLiteral("checkBox_Faces_2"));
        checkBox_Faces_2->setGeometry(QRect(20, 190, 86, 20));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Mesh_Computational_Geometry", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MainWindow", "PushButton", Q_NULLPTR));
        checkBox_Vertices->setText(QApplication::translate("MainWindow", "Vertices", Q_NULLPTR));
        checkBox_Edges->setText(QApplication::translate("MainWindow", "Edges", Q_NULLPTR));
        checkBox_Faces->setText(QApplication::translate("MainWindow", "Faces", Q_NULLPTR));
        checkBox_Faces_2->setText(QApplication::translate("MainWindow", "Laplacian", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
