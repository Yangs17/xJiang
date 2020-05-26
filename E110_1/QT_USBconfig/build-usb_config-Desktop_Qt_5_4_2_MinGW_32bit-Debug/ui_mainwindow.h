/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QComboBox *connect_config_Combo_Box;
    QPushButton *creat_config_file_pushButton;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *IP_lineEdit;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *mask_lineEdit;
    QLabel *label_5;
    QLineEdit *gatway_lineEdit;
    QLineEdit *file_path_lineEdit;
    QPushButton *choose_file_pushButton;
    QLabel *label_6;
    QRadioButton *dhcp_radioButton;
    QLabel *label_7;
    QLabel *label_8;
    QLineEdit *login_lineEdit;
    QLabel *label_9;
    QLineEdit *passwaord_lineEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(596, 321);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        connect_config_Combo_Box = new QComboBox(centralWidget);
        connect_config_Combo_Box->setObjectName(QStringLiteral("connect_config_Combo_Box"));
        connect_config_Combo_Box->setGeometry(QRect(150, 10, 91, 22));
        creat_config_file_pushButton = new QPushButton(centralWidget);
        creat_config_file_pushButton->setObjectName(QStringLiteral("creat_config_file_pushButton"));
        creat_config_file_pushButton->setGeometry(QRect(160, 250, 101, 23));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 80, 91, 16));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(40, 10, 91, 16));
        IP_lineEdit = new QLineEdit(centralWidget);
        IP_lineEdit->setObjectName(QStringLiteral("IP_lineEdit"));
        IP_lineEdit->setGeometry(QRect(150, 120, 113, 20));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(40, 120, 91, 16));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(40, 150, 91, 16));
        mask_lineEdit = new QLineEdit(centralWidget);
        mask_lineEdit->setObjectName(QStringLiteral("mask_lineEdit"));
        mask_lineEdit->setGeometry(QRect(150, 150, 113, 20));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(40, 180, 91, 16));
        gatway_lineEdit = new QLineEdit(centralWidget);
        gatway_lineEdit->setObjectName(QStringLiteral("gatway_lineEdit"));
        gatway_lineEdit->setGeometry(QRect(150, 180, 113, 20));
        file_path_lineEdit = new QLineEdit(centralWidget);
        file_path_lineEdit->setObjectName(QStringLiteral("file_path_lineEdit"));
        file_path_lineEdit->setGeometry(QRect(40, 220, 411, 20));
        choose_file_pushButton = new QPushButton(centralWidget);
        choose_file_pushButton->setObjectName(QStringLiteral("choose_file_pushButton"));
        choose_file_pushButton->setGeometry(QRect(470, 220, 91, 23));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(40, 50, 111, 16));
        dhcp_radioButton = new QRadioButton(centralWidget);
        dhcp_radioButton->setObjectName(QStringLiteral("dhcp_radioButton"));
        dhcp_radioButton->setGeometry(QRect(178, 80, 61, 20));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(290, 60, 271, 16));
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(290, 90, 91, 16));
        login_lineEdit = new QLineEdit(centralWidget);
        login_lineEdit->setObjectName(QStringLiteral("login_lineEdit"));
        login_lineEdit->setGeometry(QRect(290, 120, 271, 20));
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(290, 150, 91, 16));
        passwaord_lineEdit = new QLineEdit(centralWidget);
        passwaord_lineEdit->setObjectName(QStringLiteral("passwaord_lineEdit"));
        passwaord_lineEdit->setGeometry(QRect(290, 180, 271, 20));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 596, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        connect_config_Combo_Box->clear();
        connect_config_Combo_Box->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "eth", 0)
         << QApplication::translate("MainWindow", "wifi", 0)
         << QApplication::translate("MainWindow", "eth or wifi", 0)
        );
        creat_config_file_pushButton->setText(QApplication::translate("MainWindow", "\347\224\237\346\210\220\351\205\215\347\275\256\346\226\207\344\273\266", 0));
        label->setText(QApplication::translate("MainWindow", "DHCP Config\357\274\232", 0));
        label_2->setText(QApplication::translate("MainWindow", "Connect Config\357\274\232", 0));
        label_3->setText(QApplication::translate("MainWindow", "IP Address\357\274\232", 0));
        label_4->setText(QApplication::translate("MainWindow", "Mask Address\357\274\232", 0));
        label_5->setText(QApplication::translate("MainWindow", "Gatway Address\357\274\232", 0));
        choose_file_pushButton->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\346\226\207\344\273\266\350\267\257\345\276\204", 0));
        label_6->setText(QApplication::translate("MainWindow", "Ethernet Config\357\274\232", 0));
        dhcp_radioButton->setText(QString());
        label_7->setText(QApplication::translate("MainWindow", "Wifi Config\357\274\232\357\274\210Enter only 24 characters\357\274\211", 0));
        label_8->setText(QApplication::translate("MainWindow", "Login ID\357\274\232", 0));
        label_9->setText(QApplication::translate("MainWindow", "password\357\274\232", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
