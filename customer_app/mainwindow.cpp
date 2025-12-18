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
#include "transaction.h"
#include "transactioncsvlogger.h"
#include <QProcess>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), isTimerActive(false)
{
    ui->setupUi(this);

    // === Initialize the Fraud Detector (C++ Heuristic) ===
    fraudDetector = new FraudDetector();
    otpAttempts = 0;
    lastOtpTime = 0.0;

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
    delete fraudDetector;
    delete ui;
}

// Simple mock verification function
bool isverified(){ return true; }

// Helper: Mock history
TransactionRecord MainWindow::getLastTransaction(int index) {
    TransactionRecord rec;
    rec.amount = 50.0 + (index * 10);
    rec.merchant = "Amazon";
    return rec;
}

std::vector<TransactionRecord> MainWindow::getLastThreeTransactions(const QString& userId) {
    std::vector<TransactionRecord> transactions;
    QFile file("data/transaction_data.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return transactions;
    }

    QTextStream in(&file);
    QString header = in.readLine();
    std::vector<TransactionRecord> userTransactions;

    while (!in.atEnd()) {
        QString line = in.readLine();
        TransactionRecord record = parseTransactionLine(line);
        if (QString::fromStdString(record.userId) == userId) {
            userTransactions.push_back(record);
        }
    }
    file.close();

    int count = userTransactions.size();
    if (count >= 3) {
        transactions.push_back(userTransactions[count - 1]);
        transactions.push_back(userTransactions[count - 2]);
        transactions.push_back(userTransactions[count - 3]);
    } else {
        if (count >= 1) transactions.push_back(userTransactions[count - 1]);
        else transactions.push_back(TransactionRecord());

        if (count >= 2) transactions.push_back(userTransactions[count - 2]);
        else transactions.push_back(TransactionRecord());

        transactions.push_back(TransactionRecord());
    }
    return transactions;
}

TransactionRecord MainWindow::parseTransactionLine(const QString& line) {
    TransactionRecord record;
    QStringList fields = line.split(',');
    if (fields.size() < 10) return record;

    record.transactionId = fields[0].toStdString();
    record.userId = fields[1].toStdString();
    record.amount = fields[2].toDouble();
    record.merchant = fields[7].toStdString();
    record.category = fields[8].toStdString();
    record.timestamp = fields[4].toStdString();
    record.hourOfDay = fields[15].toInt();
    return record;
}

void MainWindow::goNext(){
    int count = (ui->stackedWidget->count())-1;
    int index = ui->stackedWidget->currentIndex();

    // --- STEP 0: LOGIN PAGE ---
    if(index==0){
        userId = ui->userIdInput->text();
        password = ui->passwordInput->text();
        if(userId.isEmpty() || password.isEmpty()){
            ui->statusLabel->setText("Enter valid User ID and Password.");
            return;
        }
        QString modelPath = "data/models/" + userId + "_model.json";
        fraudDetector->loadModel(modelPath.toStdString());
    }
    // --- STEP 1: TRANSACTION DETAILS ---
    else if(index==1){
        recipientId = ui->recipientIdInput->text();
        amount = ui->amountInput->value();
        if(recipientId.isEmpty() || amount == 0){
            ui->statusLabel->setText("Enter valid details.");
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
        if(isTimerActive) {
            isTimerActive = false;
            lastOtpTime = otpTimer.elapsed() / 1000.0;
        }
    }

    // Move to next page
    ui->stackedWidget->setCurrentIndex(index+1);
    ui->progressBar->setValue((index+1)*100/count);
    ui->statusLabel->clear();

    // --- FINAL STEP: PROCESSING ---
    if(index+1 == ui->stackedWidget->count()-1){
        if(isverified()){

            // 1. Prepare Snapshot
            BiometricSnapshot snapshot;
            snapshot.userId = userId.toStdString();
            snapshot.sessionId = "SESSION_" + std::to_string(std::time(nullptr));
            snapshot.timestamp = std::time(nullptr);
            snapshot.otpResponseTime = lastOtpTime;
            snapshot.otpAttemptsCount = otpAttempts;
            snapshot.totalOtpsSent = 1;
            snapshot.currentAmount = amount;
            snapshot.currentMerchant = recipientId.toStdString();
            snapshot.currentCategory = "Clothing";
            snapshot.currentHourOfDay = QTime::currentTime().hour();

            std::vector<TransactionRecord> lastTransactions = getLastThreeTransactions(userId);
            if (lastTransactions.size() > 0) snapshot.lastTxn1 = lastTransactions[0];
            if (lastTransactions.size() > 1) snapshot.lastTxn2 = lastTransactions[1];
            if (lastTransactions.size() > 2) snapshot.lastTxn3 = lastTransactions[2];

            snapshot.populateMockKeystrokeData();
            snapshot.calculateDerivedFeatures();

            BiometricCSVLogger::saveSnapshot(snapshot);

            Transaction txn(
                "TXN_" + QString::number(std::time(nullptr)), userId, amount,
                "\"ONLINE\"",                      // <--- CHANGED from "TRANSFER" to match CLI
                QDateTime::currentDateTime(),
                3000.0,                        // <--- Account Balance matches CLI
                "\"Laptop\"",
                "\"Mumbai\"",
                "\"Clothing\"",                    // <--- CHANGED from "Transfer" to match CLI
                false,
                5.0,
                "\"OTP\"",
                4,                             // <--- Daily Count matches CLI
                20.0,                          // <--- Avg Amount matches CLI
                0                              // <--- Failed Count matches CLI
                );
            TransactionCSVLogger::saveTransaction(txn);
            // =========================================================
            //               PYTHON PREDICTION LOGIC
            // =========================================================

            // Absolute path to Python in venv
            QString pythonPath = "../../venv/Scripts/python.exe";
            QString scriptPath = "predict.py";

            QStringList args;
            args << "-u" << scriptPath
                 // Numerical Features
                 << QString::number(txn.transactionAmount())
                 << QString::number(txn.accountBalance())
                 << QString::number(txn.dailyTransactionCount())
                 << QString::number(txn.avgTransactionAmount7d())
                 << QString::number(txn.failedTransactionCount7d())
                 << QString::number(txn.transactionDistance())
                 << QString::number(txn.hour())
                 << QString::number(txn.day())
                 << QString::number(txn.month())
                 << QString::number(txn.dayOfWeek())
                 // Categorical Features
                 << txn.transactionType()
                 << txn.deviceType()
                 << txn.location()
                 << txn.merchantCategory()
                 << txn.authenticationMethod(); // <--- STOPPED HERE as requested

            QProcess process;
            process.start(pythonPath, args);

            qDebug() << "Calling Python model...";
            process.waitForFinished(-1);

            QString output = process.readAllStandardOutput().trimmed();
            QString error = process.readAllStandardError().trimmed();

            qDebug() << "Python Output:" << output;
            if(!error.isEmpty()) qDebug() << "Python Stderr:" << error;

            bool ok;
            double fraudProb = output.toDouble(&ok)/9.0;
            if(!ok) fraudProb = 0.0;

            // =========================================================
            //               UNIFIED UI UPDATE
            // =========================================================

            double bioScore = fraudDetector->detectFraud(snapshot);
            bool isBioFlagged = snapshot.isFlagged;

            double aiThreshold = 0.7;
            bool isAiFlagged = (fraudProb >= aiThreshold);

            if (isAiFlagged || isBioFlagged) {
                QString reason = "";
                if (isAiFlagged) reason += "Transaction Pattern (AI)";
                if (isAiFlagged && isBioFlagged) reason += " & ";
                if (isBioFlagged) reason += "Biometric/Behavioral Mismatch";

                ui->paymentSuccess->setText(
                    QString("⚠️ TRANSACTION FLAGGED\n\n"
                            "Reason: %1\n"
                            "AI Probability: %2%\n"
                            "Biometric Risk: %3")
                        .arg(reason)
                        .arg(fraudProb * 100, 0, 'f', 2)
                        .arg(bioScore)
                    );
                ui->paymentSuccess->setStyleSheet("color: red; font-weight: bold; font-size: 14px;");
            } else {
                ui->paymentSuccess->setText(
                    QString("✅ Payment Successful!\n\n"
                            "AI Probability: %1% (Safe)\n"
                            "Biometric Risk: %2 (Safe)")
                        .arg(fraudProb * 100, 0, 'f', 2)
                        .arg(bioScore)
                    );
                ui->paymentSuccess->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
            }
        }
    }
}

void MainWindow::goBack()
{
    int index = ui->stackedWidget->currentIndex();
    if(index > 0) ui->stackedWidget->setCurrentIndex(index - 1);
}

void MainWindow::on_getOtp_clicked(){}

void MainWindow::on_verify_clicked()
{
    if(isTimerActive) {
        isTimerActive = false;
        lastOtpTime = otpTimer.elapsed() / 1000.0;
        ui->next_3->setEnabled(true);
        ui->verify->setEnabled(false);
    }
}

void MainWindow::on_next_3_clicked()
{
    otpTimer.start();
    isTimerActive = true;
    otpAttempts++;
    ui->next_3->setEnabled(false);
    ui->verify->setEnabled(true);
}
