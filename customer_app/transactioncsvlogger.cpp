#include "transactioncsvlogger.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>

bool TransactionCSVLogger::fileExists(const std::string& filename)
{
    return QFile::exists(QString::fromStdString(filename));
}

bool TransactionCSVLogger::saveTransaction(
    const Transaction& txn,
    const std::string& filename)
{
    // ---------- Resolve Absolute Path ----------
    QString qfilename = QString::fromStdString(filename);
    QString absolutePath;

    if (QFileInfo(qfilename).isAbsolute()) {
        absolutePath = qfilename;
    } else {
        absolutePath = QCoreApplication::applicationDirPath() + "/" + qfilename;
    }

    qDebug() << "[TransactionCSVLogger] Absolute path:" << absolutePath;

    // ---------- Ensure Directory Exists ----------
    QDir dir;
    QString dirPath = absolutePath.left(absolutePath.lastIndexOf('/'));

    if (!dir.exists(dirPath)) {
        if (!dir.mkpath(dirPath)) {
            qCritical() << "[TransactionCSVLogger] Failed to create directory!";
            return false;
        }
    }

    // ---------- Header Logic (FIXED) ----------
    bool needsHeader =
        !fileExists(filename) || QFile(absolutePath).size() == 0;

    // ---------- Open File ----------
    QFile file(absolutePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qCritical() << "[TransactionCSVLogger] Cannot open file:"
                    << file.errorString();
        return false;
    }

    QTextStream stream(&file);

    // ---------- HEADER ----------
    if (needsHeader) {
        stream
            << "Transaction_ID,"
            << "User_ID,"
            << "Transaction_Amount,"
            << "Transaction_Type,"
            << "Account_Balance,"
            << "Device_Type,"
            << "Location,"
            << "Merchant_Category,"
            << "IP_Address_Flag,"
            << "Daily_Transaction_Count,"
            << "Avg_Transaction_Amount_7d,"
            << "Failed_Transaction_Count_7d,"
            << "Transaction_Distance,"
            << "Authentication_Method,"
            << "Hour,"
            << "Day,"
            << "Month,"
            << "DayOfWeek,";
        stream.flush();
    }

    // ---------- DATA ----------
    stream
        << txn.transactionId() << ","
        << txn.userId() << ","
        << txn.transactionAmount() << ","
        << txn.transactionType() << ","
        << txn.accountBalance() << ","
        << txn.deviceType() << ","
        << txn.location() << ","
        << txn.merchantCategory() << ","
        << (txn.ipAddressFlag() ? 1 : 0) << ","
        << txn.dailyTransactionCount() << ","
        << txn.avgTransactionAmount7d() << ","
        << txn.failedTransactionCount7d() << ","
        << txn.transactionDistance() << ","
        << txn.authenticationMethod() << ","
        << txn.hour() << ","
        << txn.day() << ","
        << txn.month() << ","
        << txn.dayOfWeek()
        << "\n";

    stream.flush();
    file.close();

    qDebug() << "[TransactionCSVLogger] Transaction logged successfully";
    return true;
}
