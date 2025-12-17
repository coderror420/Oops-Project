#include "transaction.h"

Transaction::Transaction()
    : m_transactionAmount(0.0),
    m_accountBalance(0.0),
    m_ipAddressFlag(false),
    m_dailyTransactionCount(0),
    m_avgTransactionAmount7d(0.0),
    m_failedTransactionCount7d(0),
    m_transactionDistance(0.0),
    m_isWeekend(false)
{
}

Transaction::Transaction(const QString& transactionId,
                         const QString& userId,
                         double amount,
                         const QString& transactionType,
                         const QDateTime& timestamp,
                         double accountBalance,
                         const QString& deviceType,
                         const QString& location,
                         bool ipAddressFlag,
                         const QString& merchantCategory,
                         double transactionDistance,
                         const QString& authenticationMethod,
                         int dailyTransactionCount,
                         double avgTransactionAmount7d,
                         int failedTransactionCount7d)
    : m_transactionId(transactionId),
    m_userId(userId),
    m_transactionAmount(amount),
    m_transactionType(transactionType),
    m_timestamp(timestamp),
    m_accountBalance(accountBalance),
    m_deviceType(deviceType),
    m_location(location),
    m_ipAddressFlag(ipAddressFlag),
    m_merchantCategory(merchantCategory),
    m_transactionDistance(transactionDistance),
    m_authenticationMethod(authenticationMethod),
    m_dailyTransactionCount(dailyTransactionCount),
    m_avgTransactionAmount7d(avgTransactionAmount7d),
    m_failedTransactionCount7d(failedTransactionCount7d)
{
    deriveTimeFeatures();
}

void Transaction::deriveTimeFeatures()
{
    if (!m_timestamp.isValid()) {
        m_hour = m_day = m_month = m_dayOfWeek = 0;
        m_isWeekend = false;
        return;
    }

    m_hour = m_timestamp.time().hour();
    m_day = m_timestamp.date().day();
    m_month = m_timestamp.date().month();
    m_dayOfWeek = m_timestamp.date().dayOfWeek();
    m_isWeekend = (m_dayOfWeek >= 6);
}

/* Getters */
QString Transaction::transactionId() const { return m_transactionId; }
QString Transaction::userId() const { return m_userId; }
double Transaction::transactionAmount() const { return m_transactionAmount; }
QString Transaction::transactionType() const { return m_transactionType; }
QDateTime Transaction::timestamp() const { return m_timestamp; }
double Transaction::accountBalance() const { return m_accountBalance; }
QString Transaction::deviceType() const { return m_deviceType; }
QString Transaction::location() const {return m_location;}
QString Transaction::merchantCategory() const { return m_merchantCategory; }
bool Transaction::ipAddressFlag() const { return m_ipAddressFlag; }
int Transaction::dailyTransactionCount() const { return m_dailyTransactionCount; }
double Transaction::avgTransactionAmount7d() const { return m_avgTransactionAmount7d; }
int Transaction::failedTransactionCount7d() const { return m_failedTransactionCount7d; }
double Transaction::transactionDistance() const { return m_transactionDistance; }
QString Transaction::authenticationMethod() const { return m_authenticationMethod; }
int Transaction::hour() const { return m_hour; }
int Transaction::day() const { return m_day; }
int Transaction::month() const { return m_month; }
int Transaction::dayOfWeek() const { return m_dayOfWeek; }
bool Transaction::isWeekend() const { return m_isWeekend; }
