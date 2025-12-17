#include<bits/stdc++.h>
#include "Transaction.h"
using namespace std;


bool ruleHighAmountOutsideCity(const Transaction& t) {
    return t.getAmount() > 100000 && t.isOutsideHomeCity();
}

// Rule 2: High Transaction Velocity
bool ruleHighVelocity(const Transaction& t) {
    return t.getVelocity() >= 3;
}

// Rule 3: Impossible Travel Distance
bool ruleImpossibleTravel(const Transaction& t) {
    return t.getDistance() > 500;
}

// Rule 4: Odd Hour + New Merchant
bool ruleOddHourNewMerchant(const Transaction& t) {
    int h = t.getHour();
    return (h < 6 || h > 23) && t.isNewMerchantTxn();
}

// Rule Scoring
int calculateRuleScore(const Transaction& t) {
    int score = 0;

    if (ruleHighAmountOutsideCity(t)) score += 40;
    if (ruleHighVelocity(t)) score += 25;
    if (ruleImpossibleTravel(t)) score += 30;
    if (ruleOddHourNewMerchant(t)) score += 15;

    return score;
}

// Rule Based Decision
string ruleDecision(const Transaction& t) {
    int score = calculateRuleScore(t);

    if (score >= 80) return "BLOCK";
    if (score >= 50) return "OTP_VERIFICATION";
    return "ALLOW";
}

//Main function
int main() {
    Transaction txn(
        120000,  // amount
        2,       // hour
        true,    // outside home city
        true,    // new merchant
        4,       // transactions last 10 min
        700      // distance in km
    );

    int score = calculateRuleScore(txn);
    string decision = ruleDecision(txn);

    cout << "Rule Score: " << score << endl;
    cout << "Decision: " << decision << endl;

    return 0;
}