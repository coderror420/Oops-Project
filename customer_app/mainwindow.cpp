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

std::vector<TransactionRecord> MainWindow::getLastThreeTransactions(const QString& userId) {
    std::vector<TransactionRecord> transactions;

    QFile file("data/transaction_data.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open transaction_data.csv";
        // Return empty vector - will use defaults
        return transactions;
    }

    QTextStream in(&file);
    QString header = in.readLine(); // Skip header

    // Read all transactions for this user
    std::vector<TransactionRecord> userTransactions;

    while (!in.atEnd()) {
        QString line = in.readLine();
        TransactionRecord record = parseTransactionLine(line);

        // Only keep transactions for this user
        if (QString::fromStdString(record.userId) == userId) {
            userTransactions.push_back(record);
        }
    }

    file.close();

    // Get last 3 transactions (most recent)
    int count = userTransactions.size();
    if (count >= 3) {
        transactions.push_back(userTransactions[count - 1]); // Most recent
        transactions.push_back(userTransactions[count - 2]);
        transactions.push_back(userTransactions[count - 3]);
    } else if (count == 2) {
        transactions.push_back(userTransactions[count - 1]);
        transactions.push_back(userTransactions[count - 2]);
        transactions.push_back(TransactionRecord()); // Empty
    } else if (count == 1) {
        transactions.push_back(userTransactions[count - 1]);
        transactions.push_back(TransactionRecord());
        transactions.push_back(TransactionRecord());
    } else {
        // No history - return 3 empty records
        transactions.push_back(TransactionRecord());
        transactions.push_back(TransactionRecord());
        transactions.push_back(TransactionRecord());
    }

    qDebug() << "Loaded" << transactions.size() << "transaction history records for user" << userId;

    return transactions;
}

TransactionRecord MainWindow::parseTransactionLine(const QString& line) {
    TransactionRecord record;

    QStringList fields = line.split(',');

    if (fields.size() < 10) {
        qWarning() << "Invalid CSV line:" << line;
        return record;
    }

    // Parse based on your CSV format
    // Adjust indices based on your actual CSV column order

    record.transactionId = fields[0].toStdString();  // Transaction_ID
    record.userId = fields[1].toStdString();         // User_ID
    record.amount = fields[2].toDouble();            // Transaction_Amount
    record.merchant = fields[7].toStdString();       // Merchant (or use appropriate field)
    record.category = fields[8].toStdString();       // Category
    record.timestamp = fields[4].toStdString();      // Timestamp
    record.hourOfDay = fields[15].toInt();           // Hour (if available)

    return record;
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

            // ================= BIOMETRIC SNAPSHOT =================
            BiometricSnapshot snapshot;
            snapshot.userId = userId.toStdString();
            snapshot.sessionId = "SESSION_" + std::to_string(std::time(nullptr));
            snapshot.timestamp = std::time(nullptr);

            snapshot.otpResponseTime = lastOtpTime;
            snapshot.otpAttemptsCount = otpAttempts;
            snapshot.totalOtpsSent = 1;

            snapshot.currentAmount = amount;
            snapshot.currentMerchant = recipientId.toStdString();
            snapshot.currentCategory = "Transfer";
            snapshot.currentHourOfDay = QTime::currentTime().hour();

            std::vector<TransactionRecord> lastTransactions = getLastThreeTransactions(userId);

            if (lastTransactions.size() >= 3) {
                snapshot.lastTxn1 = lastTransactions[0];
                snapshot.lastTxn2 = lastTransactions[1];
                snapshot.lastTxn3 = lastTransactions[2];
            } else {
                // If not enough history, use empty records
                if (lastTransactions.size() >= 1) snapshot.lastTxn1 = lastTransactions[0];
                if (lastTransactions.size() >= 2) snapshot.lastTxn2 = lastTransactions[1];
            }

            qDebug() << "Last 3 transactions loaded:";
            if (snapshot.lastTxn1.amount > 0)
                qDebug() << "  Txn1:" << snapshot.lastTxn1.amount << QString::fromStdString(snapshot.lastTxn1.merchant);
            if (snapshot.lastTxn2.amount > 0)
                qDebug() << "  Txn2:" << snapshot.lastTxn2.amount << QString::fromStdString(snapshot.lastTxn2.merchant);
            if (snapshot.lastTxn3.amount > 0)
                qDebug() << "  Txn3:" << snapshot.lastTxn3.amount << QString::fromStdString(snapshot.lastTxn3.merchant);

            snapshot.populateMockKeystrokeData();
            snapshot.calculateDerivedFeatures();

            double fraudScore = fraudDetector->detectFraud(snapshot);

            BiometricCSVLogger::saveSnapshot(snapshot);

            // ================= TRANSACTION OBJECT =================
            Transaction txn(
                "TXN_" + QString::number(std::time(nullptr)),   // Transaction_ID
                userId,                                         // User_ID
                amount,                                         // Transaction_Amount
                "TRANSFER",                                     // Transaction_Type
                QDateTime::currentDateTime(),                   // Timestamp
                10000.0,                                        // Account_Balance (mock)
                "Laptop",                                       // Device_Type
                "Mumbai",                                       //location

                "Transfer",                                    // Merchant_Category
                false,                                          //previousfraudulent
                5.0,                                            // Transaction_Distance (km)
                "OTP",                                          // Authentication_Method
                3,                                              // Daily_Transaction_Count
                1200.0,                                         // Avg_Transaction_Amount_7d
                0                                                // Failed_Transaction_Count_7d
                );

            TransactionCSVLogger::saveTransaction(txn);
            // ================= PYTHON PREDICTION =================
            QString pythonPath = "python"; // or full path
            QString scriptPath = "predict.py";

            QStringList args;
            args << scriptPath
                 << txn.transactionId()
                 << txn.userId()
                 << QString::number(txn.transactionAmount())
                 << txn.transactionType()
                 << QString::number(txn.accountBalance())
                 << txn.deviceType()
                 << txn.location()
                 << txn.merchantCategory()
                 << (txn.previousFraudulentActivity() ? "1" : "0")
                 << QString::number(txn.dailyTransactionCount())
                 << QString::number(txn.avgTransactionAmount7d())
                 << QString::number(txn.failedTransactionCount7d())
                 << QString::number(txn.transactionDistance())
                 << txn.authenticationMethod()
                 << QString::number(txn.hour())
                 << QString::number(txn.day())
                 << QString::number(txn.month())
                 << QString::number(txn.dayOfWeek());

            QProcess process;
            process.start(pythonPath, args);
            process.waitForFinished(-1);

            QString output = process.readAllStandardOutput().trimmed();
            bool ok;
            double fraudProb = output.toDouble(&ok);

            if(!ok) {
                qWarning() << "[Python Prediction] Failed to parse probability:" << output;
                fraudProb = 0.0;
            }

            // ================= UI RESULT =================
            double threshold = 0.7; // configurable threshold for fraud
            if(fraudProb >= threshold) {
                ui->paymentSuccess->setText(
                    QString("⚠️ TRANSACTION FLAGGED\n\nFraud Probability: %1%")
                        .arg(fraudProb * 100, 0, 'f', 2)
                    );
                ui->paymentSuccess->setStyleSheet("color:red; font-weight:bold;");
            } else {
                ui->paymentSuccess->setText(
                    QString("Payment successful!\n\nUser: %1\nAmount: %2\nReceiver: %3\nFraud Probability: %4% (Safe)")
                        .arg(userId)
                        .arg(amount)
                        .arg(recipientId)
                        .arg(fraudProb * 100, 0, 'f', 2)
                    );
                ui->paymentSuccess->setStyleSheet("color:white;");
            }


            // ================= UI RESULT =================
            if(snapshot.isFlagged) {
                ui->paymentSuccess->setText(
                    "⚠️ TRANSACTION FLAGGED\n\n"
                    "Reason: Unusual Behavior Detected\n"
                    "Fraud Score: " + QString::number(fraudScore)
                    );
                ui->paymentSuccess->setStyleSheet("color: red; font-weight: bold;");
            } else {
                ui->paymentSuccess->setText(
                    "Payment successful!\n\n"
                    "User: " + userId +
                    "\nAmount: " + QString::number(amount) +
                    "\nReceiver: " + recipientId +
                    "\nRisk Score: " + QString::number(fraudScore) + " (Safe)"
                    );
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
void MainWindow::on_getOtp_clicked(){

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


