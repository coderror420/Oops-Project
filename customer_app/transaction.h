#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDateTime>

class Transaction
{
private:
    QString m_transactionId;
    QString m_userId;

    double m_transactionAmount;
    QString m_transactionType;
    QDateTime m_timestamp;
    double m_accountBalance;

    QString m_deviceType;
    QString m_merchantCategory;
    QString m_location;
    // bool m_ipAddressFlag;
    bool m_previousFraudulentActivity;

    int m_dailyTransactionCount;
    double m_avgTransactionAmount7d;
    int m_failedTransactionCount7d;

    double m_transactionDistance;
    QString m_authenticationMethod;

    // Derived
    int m_hour;
    int m_day;
    int m_month;
    int m_dayOfWeek;

public:
    Transaction();

    Transaction(const QString& transactionId,
                const QString& userId,
                double amount,
                const QString& transactionType,
                const QDateTime& timestamp,
                double accountBalance,
                const QString& deviceType,
                const QString& location,
                const QString& merchantCategory,
                const bool& previousFraudulentActivity,
                double transactionDistance,
                const QString& authenticationMethod,
                int dailyTransactionCount,
                double avgTransactionAmount7d,
                int failedTransactionCount7d);

    // Getters (DATASET ORDER)
    QString transactionId() const;
    QString userId() const;
    double transactionAmount() const;
    QString transactionType() const;
    QDateTime timestamp() const;
    double accountBalance() const;
    QString deviceType() const;
    QString location() const;
    QString merchantCategory() const;
    bool previousFraudulentActivity() const;
    // bool ipAddressFlag() const;
    int dailyTransactionCount() const;
    double avgTransactionAmount7d() const;
    int failedTransactionCount7d() const;
    double transactionDistance() const;
    QString authenticationMethod() const;

    int hour() const;
    int day() const;
    int month() const;
    int dayOfWeek() const;
    void deriveTimeFeatures();
};

#endif
