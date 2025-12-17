#include "transactioncsvlogger.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

bool TransactionCSVLogger::fileExists(const std::string& filename)
{
    return QFile::exists(QString::fromStdString(filename));
}

bool TransactionCSVLogger::saveTransaction(
    const Transaction& txn,
    const std::string& filename)
{
    // Convert to absolute path
    QString qfilename = QString::fromStdString(filename);
    QString absolutePath;

    if (QFileInfo(qfilename).isAbsolute()) {
        absolutePath = qfilename;
    } else {
        absolutePath = QCoreApplication::applicationDirPath() + "/" + qfilename;
    }

    qDebug() << "[TransactionCSVLogger] Working directory:"
             << QCoreApplication::applicationDirPath();
    qDebug() << "[TransactionCSVLogger] Absolute path:" << absolutePath;

    // Ensure directory exists
    QDir dir;
    QString dirPath = absolutePath.left(absolutePath.lastIndexOf('/'));

    if (!dir.exists(dirPath)) {
        bool created = dir.mkpath(dirPath);
        qDebug() << "[TransactionCSVLogger] Directory created:"
                 << created << "Path:" << dirPath;
        if (!created) {
            qCritical() << "[TransactionCSVLogger] FAILED to create directory!";
            return false;
        }
    }

    bool needsHeader = !fileExists(filename);
    qDebug() << "[TransactionCSVLogger] File exists:" << !needsHeader;

    QFile file(absolutePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qCritical() << "[TransactionCSVLogger] ERROR: Cannot open file!";
        qCritical() << "[TransactionCSVLogger] Error:" << file.errorString();
        return false;
    }

    QTextStream stream(&file);

    // ---- HEADER ----
    if (needsHeader) {
        qDebug() << "[TransactionCSVLogger] Writing header...";
        stream
            << "transaction_amount,"
            << "transaction_type,"
            << "account_balance,"
            << "timestamp,"
            << "hour,day,month,day_of_week,"
            << "device_type,"
            << "ip_address_flag,"
            << "merchant_category,"
            << "transaction_distance,"
            << "authentication_method,"
            << "daily_transaction_count,"
            << "avg_transaction_amount_7d,"
            << "failed_transaction_count_7d\n";
        stream.flush();
    }

    // ---- DATA ROW ----
    qDebug() << "[TransactionCSVLogger] Writing data row...";
    stream
        << txn.transactionAmount() << ","
        << txn.transactionType() << ","
        << txn.accountBalance() << ","
        << txn.timestamp().toSecsSinceEpoch() << ","
        << txn.hour() << ","
        << txn.day() << ","
        << txn.month() << ","
        << txn.dayOfWeek() << ","
        << txn.deviceType() << ","
        << (txn.ipAddressFlag() ? 1 : 0) << ","
        << txn.merchantCategory() << ","
        << txn.transactionDistance() << ","
        << txn.authenticationMethod() << ","
        << txn.dailyTransactionCount() << ","
        << txn.avgTransactionAmount7d() << ","
        << txn.failedTransactionCount7d()
        << "\n";

    stream.flush();
    file.close();

    qDebug() << "[TransactionCSVLogger] SUCCESS! File saved to:" << absolutePath;
    qDebug() << "[TransactionCSVLogger] File size:" << file.size() << "bytes";

    return true;
}
