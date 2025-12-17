#include <iostream>
#include <string>

using namespace std;
//Transaction class
// class Transaction {
// private:
//     double amount;
//     int hour;
//     bool outsideHomeCity;
//     bool isNewMerchant;
//     int transactionsLast10Min;
//     int distanceFromLastTxn;

// public:
//     Transaction(double amt, int hr, bool outsideCity,
//                 bool newMerchant, int velocity, int distance)
//         : amount(amt),
//           hour(hr),
//           outsideHomeCity(outsideCity),
//           isNewMerchant(newMerchant),
//           transactionsLast10Min(velocity),
//           distanceFromLastTxn(distance) {}
//     //Amount of transaction
//     double getAmount() const { return amount; }
//     //Transaction hour (0-23) (usually fraud takes place in odd hour);
//     int getHour() const { return hour; }
//     //sudden geographic change lead to fraud risk
//     bool isOutsideHomeCity() const { return outsideHomeCity; }
//     //is the user familiar with the merchant
//     bool isNewMerchantTxn() const { return isNewMerchant; }
//     //frequency of transaction
//     int getVelocity() const { return transactionsLast10Min; }
//     //distance from the location of previous transaction
//     int getDistance() const { return distanceFromLastTxn; }
// };


#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include <unordered_map>

class Transaction {
public:
    // ---------- Data Members ----------
    double transaction_amount;
    std::string transaction_type;
    double account_balance;

    time_t timestamp;
    int hour;
    int day_of_week;
    bool is_weekend;

    string device_type;
    bool ip_address_flag;

    string merchant_category;
    double transaction_distance;

    string authentication_method;

    int daily_transaction_count;
    double avg_transaction_amount_7d;
    int failed_transaction_count_7d;

    // ---------- Constructor ----------
    Transaction(
        double amount,
        const string& txn_type,
        double balance,
        time_t ts,
        const string& device,
        bool ip_flag,
        const string& merchant,
        double distance,
        const string& auth_method,
        int daily_count,
        double avg_7d,
        int failed_7d
    );

    // ---------- Core Functions ----------
    void deriveTimeFeatures();
    bool isHighValue(double threshold) const;
    bool isGeoAnomalous(double distance_threshold) const;
    bool hasRecentFailures(int failure_threshold) const;
    bool isRiskyTime() const;

    // ---------- ML Integration ----------
    std::string toJSON() const;
    std::unordered_map<std::string, double> numericFeatureMap() const;

    // ---------- Utility ----------
    void print() const;
};

#endif
