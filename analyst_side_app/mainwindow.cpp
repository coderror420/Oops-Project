#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isTimerActive(false) //timer variable
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

//generate otp button ka slot
void MainWindow::on_generateOtp_clicked()
{
    otpTimer.start();
    isTimerActive = true;

    //Disable button to prevent multiple clicks
    ui->generateOtp->setEnabled(false);
    ui->submitOtp->setEnabled(true);
}

//submit otp button ka slot
void MainWindow::on_submitOtp_clicked()
{
    if(isTimerActive) {
        isTimerActive = false;
        qint64 elapsed = otpTimer.elapsed();
        double seconds = elapsed / 1000.0;

        ui->statusOtp->setText(QString("%1 seconds").arg(seconds, 0, 'f', 2));

        // ✅ Store it in your transaction object here
        // transaction.getBehavioralData().setOTPResponseTime(seconds);

        //Re-enable generate button
        ui->generateOtp->setEnabled(true);
        ui->submitOtp->setEnabled(false);
    }
}
