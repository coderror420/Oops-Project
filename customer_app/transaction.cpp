#include "transaction.h"
#include <QJsonObject>
#include <QJsonDocument>
Transaction::Transaction()
    : m_transactionAmount(0.0),
    m_accountBalance(0.0),
    m_month(0),
    m_hour(0),
    m_day(0),
    m_dayOfWeek(0),
    m_ipAddressFlag(false),
    m_transactionDistance(0.0),
    m_dailyTransactionCount(0),
    m_avgTransactionAmount7d(0.0),
    m_failedTransactionCount7d(0)
{
}

// ---------- Parameterized Constructor ----------
Transaction::Transaction(double amount,
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
                         int failedTransactionCount7d)
    : m_transactionAmount(amount),
    m_transactionType(transactionType),
    m_accountBalance(accountBalance),
    m_timestamp(timestamp),
    m_deviceType(deviceType),
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
    m_hour = m_timestamp.time().hour();
    m_day = m_timestamp.date().day();
    m_month = m_timestamp.date().month();
    m_dayOfWeek = m_timestamp.date().dayOfWeek(); // 1 = Mon, 7 = Sun
}


double Transaction::transactionAmount() const { return m_transactionAmount; }
QString Transaction::transactionType() const { return m_transactionType; }
double Transaction::accountBalance() const { return m_accountBalance; }
QDateTime Transaction::timestamp() const { return m_timestamp; }

int Transaction::hour() const { return m_hour; }
int Transaction::day() const { return m_day; }
int Transaction::month() const { return m_month; }
int Transaction::dayOfWeek() const { return m_dayOfWeek; }

QString Transaction::deviceType() const { return m_deviceType; }
bool Transaction::ipAddressFlag() const { return m_ipAddressFlag; }

QString Transaction::merchantCategory() const { return m_merchantCategory; }
double Transaction::transactionDistance() const { return m_transactionDistance; }

QString Transaction::authenticationMethod() const { return m_authenticationMethod; }

int Transaction::dailyTransactionCount() const { return m_dailyTransactionCount; }
double Transaction::avgTransactionAmount7d() const { return m_avgTransactionAmount7d; }
int Transaction::failedTransactionCount7d() const { return m_failedTransactionCount7d; }

bool Transaction::isHighValue(double threshold) const
{
    return m_transactionAmount > threshold;
}

bool Transaction::isGeoAnomalous(double distanceThreshold) const
{
    return m_transactionDistance > distanceThreshold;
}

bool Transaction::hasRecentFailures(int failureThreshold) const
{
    return m_failedTransactionCount7d >= failureThreshold;
}

bool Transaction::isRiskyTime() const
{
    return (m_hour >= 0 && m_hour <= 5);
}

QString Transaction::toJson() const
{
    QJsonObject obj;

    obj["Transaction_Amount"] = m_transactionAmount;
    obj["Account_Balance"] = m_accountBalance;
    obj["Daily_Transaction_Count"] = m_dailyTransactionCount;
    obj["Avg_Transaction_Amount_7d"] = m_avgTransactionAmount7d;
    obj["Failed_Transaction_Count_7d"] = m_failedTransactionCount7d;
    obj["Transaction_Distance"] = m_transactionDistance;

    obj["hour"] = m_hour;
    obj["day"] = m_day;
    obj["month"] = m_month;
    obj["dayofweek"] = m_dayOfWeek;

    obj["IP_Address_Flag"] = m_ipAddressFlag ? 1 : 0;

    obj["Transaction_Type"] = m_transactionType;
    obj["Device_Type"] = m_deviceType;
    obj["Merchant_Category"] = m_merchantCategory;
    obj["Authentication_Method"] = m_authenticationMethod;

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}
