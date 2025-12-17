#ifndef TRANSACTIONCSVLOGGER_H
#define TRANSACTIONCSVLOGGER_H

#include "transaction.h"
#include <string>

class TransactionCSVLogger
{
public:
    static bool saveTransaction(
        const Transaction& txn,
        const std::string& filename = "data/transaction_data.csv"
        );

    static bool fileExists(const std::string& filename);
};

#endif // TRANSACTIONCSVLOGGER_H
