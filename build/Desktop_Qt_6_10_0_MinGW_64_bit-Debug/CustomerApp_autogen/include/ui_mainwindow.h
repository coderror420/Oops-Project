/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *pageUser;
    QPushButton *next;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *userId;
    QLineEdit *userIdInput;
    QLabel *password;
    QLineEdit *passwordInput;
    QWidget *pagePay;
    QPushButton *next_3;
    QPushButton *back;
    QWidget *widget1;
    QGridLayout *gridLayout_2;
    QLabel *recipientId;
    QLineEdit *recipientIdInput;
    QLabel *amount;
    QSpinBox *amountInput;
    QWidget *pageOtp;
    QPushButton *back_2;
    QPushButton *verify;
    QWidget *widget2;
    QGridLayout *gridLayout_3;
    QLabel *otp;
    QLineEdit *otpValue;
    QPushButton *getOtp;
    QWidget *page_5;
    QLabel *paymentSuccess;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(351, 215);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(30, 20, 291, 171));
        pageUser = new QWidget();
        pageUser->setObjectName("pageUser");
        pageUser->setAutoFillBackground(false);
        next = new QPushButton(pageUser);
        next->setObjectName("next");
        next->setGeometry(QRect(179, 110, 41, 20));
        widget = new QWidget(pageUser);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(60, 50, 161, 51));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        userId = new QLabel(widget);
        userId->setObjectName("userId");

        gridLayout->addWidget(userId, 0, 0, 1, 1);

        userIdInput = new QLineEdit(widget);
        userIdInput->setObjectName("userIdInput");

        gridLayout->addWidget(userIdInput, 0, 1, 1, 1);

        password = new QLabel(widget);
        password->setObjectName("password");

        gridLayout->addWidget(password, 1, 0, 1, 1);

        passwordInput = new QLineEdit(widget);
        passwordInput->setObjectName("passwordInput");

        gridLayout->addWidget(passwordInput, 1, 1, 1, 1);

        stackedWidget->addWidget(pageUser);
        pagePay = new QWidget();
        pagePay->setObjectName("pagePay");
        next_3 = new QPushButton(pagePay);
        next_3->setObjectName("next_3");
        next_3->setGeometry(QRect(179, 100, 41, 20));
        back = new QPushButton(pagePay);
        back->setObjectName("back");
        back->setGeometry(QRect(50, 100, 41, 18));
        widget1 = new QWidget(pagePay);
        widget1->setObjectName("widget1");
        widget1->setGeometry(QRect(50, 40, 171, 51));
        gridLayout_2 = new QGridLayout(widget1);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        recipientId = new QLabel(widget1);
        recipientId->setObjectName("recipientId");

        gridLayout_2->addWidget(recipientId, 0, 0, 1, 1);

        recipientIdInput = new QLineEdit(widget1);
        recipientIdInput->setObjectName("recipientIdInput");

        gridLayout_2->addWidget(recipientIdInput, 0, 1, 1, 1);

        amount = new QLabel(widget1);
        amount->setObjectName("amount");

        gridLayout_2->addWidget(amount, 1, 0, 1, 1);

        amountInput = new QSpinBox(widget1);
        amountInput->setObjectName("amountInput");
        amountInput->setMaximum(1000000000);

        gridLayout_2->addWidget(amountInput, 1, 1, 1, 1);

        stackedWidget->addWidget(pagePay);
        pageOtp = new QWidget();
        pageOtp->setObjectName("pageOtp");
        back_2 = new QPushButton(pageOtp);
        back_2->setObjectName("back_2");
        back_2->setGeometry(QRect(51, 100, 41, 18));
        verify = new QPushButton(pageOtp);
        verify->setObjectName("verify");
        verify->setGeometry(QRect(180, 100, 41, 20));
        widget2 = new QWidget(pageOtp);
        widget2->setObjectName("widget2");
        widget2->setGeometry(QRect(51, 40, 171, 51));
        gridLayout_3 = new QGridLayout(widget2);
        gridLayout_3->setObjectName("gridLayout_3");
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        otp = new QLabel(widget2);
        otp->setObjectName("otp");

        gridLayout_3->addWidget(otp, 0, 0, 1, 1);

        otpValue = new QLineEdit(widget2);
        otpValue->setObjectName("otpValue");

        gridLayout_3->addWidget(otpValue, 0, 1, 1, 1);

        getOtp = new QPushButton(widget2);
        getOtp->setObjectName("getOtp");

        gridLayout_3->addWidget(getOtp, 1, 1, 1, 1);

        stackedWidget->addWidget(pageOtp);
        page_5 = new QWidget();
        page_5->setObjectName("page_5");
        paymentSuccess = new QLabel(page_5);
        paymentSuccess->setObjectName("paymentSuccess");
        paymentSuccess->setGeometry(QRect(10, 20, 261, 131));
        stackedWidget->addWidget(page_5);
        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName("progressBar");
        progressBar->setGeometry(QRect(0, 0, 351, 16));
        progressBar->setValue(24);
        progressBar->setTextVisible(false);
        progressBar->setInvertedAppearance(false);
        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(80, 190, 201, 20));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        next->setText(QCoreApplication::translate("MainWindow", "Next", nullptr));
        userId->setText(QCoreApplication::translate("MainWindow", "User ID:", nullptr));
        userIdInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter your user ID", nullptr));
        password->setText(QCoreApplication::translate("MainWindow", "Password:", nullptr));
        passwordInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter Password", nullptr));
        next_3->setText(QCoreApplication::translate("MainWindow", "Next", nullptr));
        back->setText(QCoreApplication::translate("MainWindow", "Back", nullptr));
        recipientId->setText(QCoreApplication::translate("MainWindow", "Recipient ID:", nullptr));
        recipientIdInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter Recipient ID", nullptr));
        amount->setText(QCoreApplication::translate("MainWindow", "Amount: ", nullptr));
        back_2->setText(QCoreApplication::translate("MainWindow", "Back", nullptr));
        verify->setText(QCoreApplication::translate("MainWindow", "Verify", nullptr));
        otp->setText(QCoreApplication::translate("MainWindow", "Enter OTP :", nullptr));
        otpValue->setText(QString());
        otpValue->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter the OTP received", nullptr));
        getOtp->setText(QCoreApplication::translate("MainWindow", "Send OTP", nullptr));
        paymentSuccess->setText(QString());
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
