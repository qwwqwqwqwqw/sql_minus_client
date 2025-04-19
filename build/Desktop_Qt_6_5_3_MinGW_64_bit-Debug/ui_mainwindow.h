/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *record;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QTextEdit *msg;
    QLabel *label;
    QLabel *label_4;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLineEdit *ip;
    QLabel *label_2;
    QPushButton *disconnect;
    QPushButton *connect;
    QLabel *label_3;
    QLineEdit *port;
    QTreeView *file;
    QTableView *table;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1284, 739);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        record = new QTextEdit(centralwidget);
        record->setObjectName("record");
        record->setGeometry(QRect(650, 490, 601, 131));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(650, 640, 241, 51));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(1020, 640, 231, 51));
        msg = new QTextEdit(centralwidget);
        msg->setObjectName("msg");
        msg->setGeometry(QRect(650, 20, 601, 431));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(650, 460, 69, 19));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 30, 271, 101));
        label_4->setStyleSheet(QString::fromUtf8("image: url(:/new/prefix1/IMG_20230725_171039.jpg);"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 150, 311, 101));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        ip = new QLineEdit(layoutWidget);
        ip->setObjectName("ip");

        gridLayout->addWidget(ip, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        disconnect = new QPushButton(layoutWidget);
        disconnect->setObjectName("disconnect");

        gridLayout->addWidget(disconnect, 2, 3, 1, 1);

        connect = new QPushButton(layoutWidget);
        connect->setObjectName("connect");

        gridLayout->addWidget(connect, 0, 3, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        port = new QLineEdit(layoutWidget);
        port->setObjectName("port");

        gridLayout->addWidget(port, 2, 1, 1, 1);

        file = new QTreeView(centralwidget);
        file->setObjectName("file");
        file->setGeometry(QRect(350, 40, 281, 231));
        table = new QTableView(centralwidget);
        table->setObjectName("table");
        table->setGeometry(QRect(10, 300, 621, 401));
        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(350, 10, 93, 28));
        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setGeometry(QRect(10, 270, 51, 28));
        pushButton_5 = new QPushButton(centralwidget);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setGeometry(QRect(60, 270, 51, 28));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\346\211\247\350\241\214", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\216\206\345\217\262\344\277\241\346\201\257", nullptr));
        label_4->setText(QString());
        label_2->setText(QCoreApplication::translate("MainWindow", "IP:", nullptr));
        disconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\346\234\215\345\212\241\345\231\250", nullptr));
        connect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "\347\273\223\346\236\204", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
