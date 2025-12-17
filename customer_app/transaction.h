#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDateTime>

class Transaction
{
private:
    // ---------- Data Members ----------
    double m_transactionAmount;
    QString m_transactionType;
    double m_accountBalance;

    QDateTime m_timestamp;
    int m_month;
    int m_hour;
    int m_day;
    int m_dayOfWeek;

    // int m_previousFraudulentActivity; add in user

    QString m_deviceType;
    // QString Location; ehh idk
    bool m_ipAddressFlag;

    QString m_merchantCategory;
    double m_transactionDistance;

    QString m_authenticationMethod;

    int m_dailyTransactionCount;
    double m_avgTransactionAmount7d;
    int m_failedTransactionCount7d;
public:
    // ---------- Constructors ----------
    Transaction();
    Transaction(double amount,
                const QString &transactionType,
                double accountBalance,
                const QDateTime &timestamp,
                const QString &deviceType,
                bool ipAddressFlag,
                const QString &merchantCategory,
                double transactionDistance,
                const QString &authenticationMethod,
                int dailyTransactionCount,
                double avgTransactionAmount7d,
                int failedTransactionCount7d);

    // ---------- Getters ----------
    double transactionAmount() const;
    QString transactionType() const;
    double accountBalance() const;
    QDateTime timestamp() const;
    int hour() const;
    int day() const;
    int month() const;
    int dayOfWeek() const;
    QString deviceType() const;
    bool ipAddressFlag() const;
    QString merchantCategory() const;
    double transactionDistance() const;
    QString authenticationMethod() const;
    int dailyTransactionCount() const;
    double avgTransactionAmount7d() const;
    int failedTransactionCount7d() const;

    // ---------- Utility Functions ----------
    void deriveTimeFeatures();
    bool isHighValue(double threshold) const;
    bool isGeoAnomalous(double distanceThreshold) const;
    bool hasRecentFailures(int failureThreshold) const;
    bool isRiskyTime() const;

    // ---------- ML Integration ----------
    QString toJson() const;


};

#endif // TRANSACTION_H
