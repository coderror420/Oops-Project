#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <QTime>
#include "FraudDetector.h"
#include "BiometricSnapshot.h"
#include "BiometricCSVLogger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), isTimerActive(false)
{
    ui->setupUi(this);

    // === NEW: Initialize the Fraud Detector ===
    fraudDetector = new FraudDetector();
    otpAttempts = 0;
    lastOtpTime = 0.0;
    // ==========================================

    ui->stackedWidget->setCurrentIndex(0);
    ui->progressBar->setValue(0);

    // Connect next buttons
    connect(ui->next, &QPushButton::clicked, this, &MainWindow::goNext);
    connect(ui->next_3, &QPushButton::clicked, this, &MainWindow::goNext);
    connect(ui->verify, &QPushButton::clicked, this, &MainWindow::goNext);

    // Connect back buttons
    connect(ui->back, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(ui->back_2, &QPushButton::clicked, this, &MainWindow::goBack);
}

MainWindow::~MainWindow()
{
    delete fraudDetector; // Clean up memory
    delete ui;
}

// Simple mock verification function
bool isverified(){ return true; }

// Helper: Mock history (You would replace this with real DB queries later)
TransactionRecord MainWindow::getLastTransaction(int index) {
    TransactionRecord rec;
    rec.amount = 50.0 + (index * 10); // Fake data
    rec.merchant = "Amazon";
    return rec;
}

void MainWindow::goNext(){
    int count = (ui->stackedWidget->count())-1;
    int index = ui->stackedWidget->currentIndex();

    // --- STEP 0: LOGIN PAGE ---
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

        // Try to load the ML model for this user
        // (Assumes you have a folder "data/models" with .json files)
        QString modelPath = "data/models/" + userId + "_model.json";
        fraudDetector->loadModel(modelPath.toStdString());
    }
    // --- STEP 1: TRANSACTION DETAILS ---
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
    // --- STEP 2: OTP PAGE ---
    else if(index==2){
        otp = ui->otpValue->text();
        if(otp.length()!=6){
            ui->statusLabel->setText("Enter valid OTP.");
            return;
        }

        // Ensure we capture the timer if the user didn't click "verify" manually
        // (In case goNext triggered before on_verify_clicked logic finished)
        if(isTimerActive) {
            isTimerActive = false;
            lastOtpTime = otpTimer.elapsed() / 1000.0;
        }
    }

    // Move to next page
    ui->stackedWidget->setCurrentIndex(index+1);
    ui->progressBar->setValue((index+1)*100/count);
    ui->statusLabel->clear();

    // --- FINAL STEP: PROCESSING & BIOMETRICS ---
    if(index+1 == ui->stackedWidget->count()-1){
        if(isverified()){

            // 1. Create the Umbrella Class (Snapshot)
            BiometricSnapshot snapshot;
            snapshot.userId = userId.toStdString();
            snapshot.sessionId = "SESSION_" + std::to_string(std::time(nullptr));
            snapshot.timestamp = std::time(nullptr);

            // 2. Fill OTP Data
            snapshot.otpResponseTime = lastOtpTime;
            snapshot.otpAttemptsCount = otpAttempts;
            snapshot.totalOtpsSent = 1; // Assuming 1 for this flow

            // 3. Fill Transaction Data
            snapshot.currentAmount = amount;
            snapshot.currentMerchant = recipientId.toStdString(); // Using recipient as merchant
            snapshot.currentCategory = "Transfer";
            snapshot.currentHourOfDay = QTime::currentTime().hour();
            snapshot.isWeekend = (QDate::currentDate().dayOfWeek() >= 6);

            // 4. Fill History (Mocked for now)
            snapshot.lastTxn1 = getLastTransaction(0);
            snapshot.lastTxn2 = getLastTransaction(1);
            snapshot.lastTxn3 = getLastTransaction(2);

            // 5. Calculate Derived Features
            snapshot.populateMockKeystrokeData();
            snapshot.calculateDerivedFeatures();

            // 6. Run Fraud Detection
            double fraudScore = fraudDetector->detectFraud(snapshot);

            // 7. Log to CSV (Crucial for future training)
            BiometricCSVLogger::saveSnapshot(snapshot);

            // 8. Display Result based on Fraud Score
            if(snapshot.isFlagged) {
                // FRAUD CASE
                ui->paymentSuccess->setText("⚠️ TRANSACTION FLAGGED\n\n"
                                            "Reason: Unusual Behavior Detected\n"
                                            "Fraud Score: " + QString::number(fraudScore));
                ui->paymentSuccess->setStyleSheet("color: red; font-weight: bold;");
            } else {
                // SUCCESS CASE
                ui->paymentSuccess->setText("Payment successful!\n\n"
                                            "User: " + userId +
                                            "\nAmount: " + QString::number(amount) +
                                            "\nReceiver: " + recipientId +
                                            "\nRisk Score: " + QString::number(fraudScore) + " (Safe)");
                ui->paymentSuccess->setStyleSheet("color: white;");
            }
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
        lastOtpTime = elapsed / 1000.0; // Store in member variable

        std::cout << lastOtpTime << std::endl;

        // Re-enable generate button
        ui->next_3->setEnabled(true);
        ui->verify->setEnabled(false);
    }
}

void MainWindow::on_next_3_clicked()
{
    otpTimer.start();
    isTimerActive = true;
    otpAttempts++; // Increment attempt counter

    // Disable button to prevent multiple clicks
    ui->next_3->setEnabled(false);
    ui->verify->setEnabled(true);
}

