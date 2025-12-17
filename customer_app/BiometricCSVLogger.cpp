#include "BiometricCSVLogger.h"
#include <QDir>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <sys/stat.h>
#include <string>

bool BiometricCSVLogger::fileExists(const std::string& filename) {
    return QFile::exists(QString::fromStdString(filename));
}

bool BiometricCSVLogger::saveSnapshot(const BiometricSnapshot& snapshot,
                                      const std::string& filename) {
    // Convert to absolute path
    QString qfilename = QString::fromStdString(filename);
    QString absolutePath;

    if (QFileInfo(qfilename).isAbsolute()) {
        absolutePath = qfilename;
    } else {
        // Use project/build directory as base
        absolutePath = QCoreApplication::applicationDirPath() + "/" + qfilename;
    }

    qDebug() << "[BiometricCSVLogger] Working directory:" << QCoreApplication::applicationDirPath();
    qDebug() << "[BiometricCSVLogger] Absolute path:" << absolutePath;
    qDebug() << "[BiometricCSVLogger] Data - userId:" << QString::fromStdString(snapshot.userId);
    qDebug() << "[BiometricCSVLogger] Data - sessionId:" << QString::fromStdString(snapshot.sessionId);

    // Ensure directory exists
    QDir dir;
    QString dirPath = absolutePath.left(absolutePath.lastIndexOf('/'));

    qDebug() << "[BiometricCSVLogger] Directory path:" << dirPath;

    if (!dir.exists(dirPath)) {
        bool created = dir.mkpath(dirPath);
        qDebug() << "[BiometricCSVLogger] Directory created:" << created << "Path:" << dirPath;
        if (!created) {
            qCritical() << "[BiometricCSVLogger] FAILED to create directory!";
            return false;
        }
    }

    bool needsHeader = !fileExists(filename);
    qDebug() << "[BiometricCSVLogger] File exists:" << !needsHeader;

    // Use Qt's QFile instead of std::ofstream
    QFile file(absolutePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qCritical() << "[BiometricCSVLogger] ERROR: Cannot open file!";
        qCritical() << "[BiometricCSVLogger] Error code:" << file.error();
        qCritical() << "[BiometricCSVLogger] Error string:" << file.errorString();
        qCritical() << "[BiometricCSVLogger] File path:" << absolutePath;
        return false;
    }

    qDebug() << "[BiometricCSVLogger] File opened successfully!";

    QTextStream stream(&file);

    if (needsHeader) {
        qDebug() << "[BiometricCSVLogger] Writing header...";
        stream << "user_id,session_id,timestamp,"
               << "avg_dwell_time,std_dwell_time,avg_flight_time,std_flight_time,"
               << "typing_speed,backspace_rate,pause_count,"
               << "otp_response_time,otp_attempts_count,total_otps_sent,"
               << "current_amount,current_merchant,current_category,current_hour_of_day,is_weekend,"
               << "last_txn1_id,last_txn1_amount,last_txn1_merchant,last_txn1_category,"
               << "last_txn2_id,last_txn2_amount,last_txn2_merchant,last_txn2_category,"
               << "last_txn3_id,last_txn3_amount,last_txn3_merchant,last_txn3_category,"
               << "amount_vs_avg_last3,time_since_last_txn,merchant_seen_before,"
               << "device_id,is_known_device,geo_distance_km,"
               << "fraud_score,is_flagged,flag_reason\n";
        stream.flush();
    }

    qDebug() << "[BiometricCSVLogger] Writing data row...";
    stream << QString::fromStdString(snapshot.userId) << ","
           << QString::fromStdString(snapshot.sessionId) << ","
           << snapshot.timestamp << ","
           << snapshot.avgDwellTime << ","
           << snapshot.stdDwellTime << ","
           << snapshot.avgFlightTime << ","
           << snapshot.stdFlightTime << ","
           << snapshot.typingSpeed << ","
           << snapshot.backspaceRate << ","
           << snapshot.pauseCount << ","
           << snapshot.otpResponseTime << ","
           << snapshot.otpAttemptsCount << ","
           << snapshot.totalOtpsSent << ","
           << snapshot.currentAmount << ","
           << QString::fromStdString(snapshot.currentMerchant) << ","
           << QString::fromStdString(snapshot.currentCategory) << ","
           << snapshot.currentHourOfDay << ","
           << snapshot.isWeekend << ","
           << QString::fromStdString(snapshot.lastTxn1.transactionId) << ","
           << snapshot.lastTxn1.amount << ","
           << QString::fromStdString(snapshot.lastTxn1.merchant) << ","
           << QString::fromStdString(snapshot.lastTxn1.category) << ","
           << QString::fromStdString(snapshot.lastTxn2.transactionId) << ","
           << snapshot.lastTxn2.amount << ","
           << QString::fromStdString(snapshot.lastTxn2.merchant) << ","
           << QString::fromStdString(snapshot.lastTxn2.category) << ","
           << QString::fromStdString(snapshot.lastTxn3.transactionId) << ","
           << snapshot.lastTxn3.amount << ","
           << QString::fromStdString(snapshot.lastTxn3.merchant) << ","
           << QString::fromStdString(snapshot.lastTxn3.category) << ","
           << snapshot.amountVsAvgLast3 << ","
           << snapshot.timeSinceLastTxn << ","
           << snapshot.merchantSeenBefore << ","
           << QString::fromStdString(snapshot.deviceId) << ","
           << snapshot.isKnownDevice << ","
           << snapshot.geoDistanceKm << ","
           << snapshot.fraudScore << ","
           << snapshot.isFlagged << ","
           << QString::fromStdString(snapshot.flagReason) << "\n";

    stream.flush();
    file.close();

    qDebug() << "[BiometricCSVLogger] SUCCESS! File saved to:" << absolutePath;
    qDebug() << "[BiometricCSVLogger] File size:" << file.size() << "bytes";

    return true;
}

void BiometricCSVLogger::writeHeader(std::ofstream& file) {
    // Deprecated - using Qt's QTextStream now
}

void BiometricCSVLogger::writeDataRow(std::ofstream& file,
                                      const BiometricSnapshot& snapshot) {
    // Deprecated - using Qt's QTextStream now
}
