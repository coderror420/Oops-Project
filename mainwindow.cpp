#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), isTimerActive(false)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->progressBar->setValue(0);
    //connect next buttons
    connect(ui->next, &QPushButton::clicked, this, &MainWindow::goNext);
    connect(ui->next_3, &QPushButton::clicked, this, &MainWindow::goNext);
    connect(ui->verify, &QPushButton::clicked, this, &MainWindow::goNext);
    //connect back buttons
    // Connect BACK buttons
    connect(ui->back, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(ui->back_2, &QPushButton::clicked, this, &MainWindow::goBack);
}

MainWindow::~MainWindow()
{
    delete ui;
}
bool isverified(){ return true;}
void MainWindow::goNext(){
    int count = (ui->stackedWidget->count())-1;
    int index = ui->stackedWidget->currentIndex();


    if(index==0){
        userId = ui->userIdInput->text();
        password = ui->passwordInput->text();
        if(userId.isEmpty() && password.isEmpty()){
            ui->statusLabel->setText("Enter valid User ID and Password.");
            return;
        }
        if(userId.isEmpty()){
            ui->statusLabel->setText("Enter valid User ID.");
            return;
        }
        if(password.isEmpty()){
            ui->statusLabel->setText("Enter valid Password.");
            return;
        }
    }
    else if(index==1){
        recipientId = ui->recipientIdInput->text();
        amount = ui->amountInput->value();
        if(recipientId.isEmpty()){
            ui->statusLabel->setText("Enter Recipient Details.");
            return;
        }
        if(amount==0){
            ui->statusLabel->setText("Enter valid amount.");
            return;
        }
    }
    else if(index==2){
        otp = ui->otpValue->text();
        if(otp.length()!=6){
            ui->statusLabel->setText("Enter valid OTP.");
            return;
        }
    }


    ui->stackedWidget->setCurrentIndex(index+1);
    ui->progressBar->setValue((index+1)*100/count);
    ui->statusLabel->clear();
    if(index+1==ui->stackedWidget->count()-1){
        if(isverified()){
            ui->paymentSuccess->setText("Payment successful!\n\n"
                                        "User: " + userId +
                                        "\nAmount: " + QString::number(amount) +
                                        "\nReceiver: " + recipientId);
        }
    }
}
void MainWindow::goBack()
{
    int index = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(index - 1);
}


void MainWindow::on_verify_clicked()
{
    if(isTimerActive) {
        isTimerActive = false;
        qint64 elapsed = otpTimer.elapsed();
        double seconds = elapsed / 1000.0;

        //ui->statusOtp->setText(QString("%1 seconds").arg(seconds, 0, 'f', 2));
        std::cout<<seconds;
        // Store it in your transaction object here
        // transaction.getBehavioralData().setOTPResponseTime(seconds);

        //Re-enable generate button
        ui->next_3->setEnabled(true);
        ui->verify->setEnabled(false);
    }
}


void MainWindow::on_next_3_clicked()
{
    otpTimer.start();
    isTimerActive = true;

    //Disable button to prevent multiple clicks
    ui->next_3->setEnabled(false);
    ui->verify->setEnabled(true);
}


