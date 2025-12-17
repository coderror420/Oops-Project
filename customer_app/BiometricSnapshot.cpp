#include "BiometricSnapshot.h"
#include <cmath>
#include <cstdlib>

BiometricSnapshot::BiometricSnapshot()
    : userId(""), sessionId(""), timestamp(0),
    avgDwellTime(0), stdDwellTime(0),
    avgFlightTime(0), stdFlightTime(0),
    typingSpeed(0), backspaceRate(0), pauseCount(0),
    otpResponseTime(0), otpAttemptsCount(0), totalOtpsSent(0),
    currentAmount(0), currentMerchant(""), currentCategory(""),
    amountVsAvgLast3(0), timeSinceLastTxn(0), merchantSeenBefore(false),
    deviceId(""), isKnownDevice(true), geoDistanceKm(0),
    fraudScore(0), isFlagged(false), flagReason("") {}

std::vector<double> BiometricSnapshot::toFeatureVector() const {
    std::vector<double> features;
    // 14 core features for ML model
    features.push_back(avgDwellTime); // 0
    features.push_back(stdDwellTime); // 1
    features.push_back(avgFlightTime); // 2
    features.push_back(stdFlightTime); // 3
    features.push_back(typingSpeed); // 4
    features.push_back(backspaceRate); // 5
    features.push_back(otpResponseTime); // 6
    features.push_back(currentAmount); // 7
    features.push_back(amountVsAvgLast3); // 8
    features.push_back(timeSinceLastTxn); // 9
    features.push_back(isKnownDevice ? 0.0 : 1.0); // 10
    features.push_back(geoDistanceKm); // 11
    features.push_back(currentHourOfDay); // 12
    features.push_back(otpAttemptsCount); // 13
    return features;
}

std::vector<std::string> BiometricSnapshot::getFeatureNames() {
    return {
        "avg_dwell_time",
        "std_dwell_time",
        "avg_flight_time",
        "std_flight_time",
        "typing_speed",
        "backspace_rate",
        "otp_response_time",
        "current_amount",
        "amount_vs_avg_last3",
        "time_since_last_txn",
        "is_unknown_device",
        "geo_distance_km",
        "current_hour_of_day",
        "otp_attempts_count"
    };
}

void BiometricSnapshot::calculateDerivedFeatures() {
    // Calculate average of last 3 transactions
    double avgLast3 = 0.0;
    int count = 0;
    if (lastTxn1.amount > 0) { avgLast3 += lastTxn1.amount; count++; }
    if (lastTxn2.amount > 0) { avgLast3 += lastTxn2.amount; count++; }
    if (lastTxn3.amount > 0) { avgLast3 += lastTxn3.amount; count++; }
    if (count > 0) {
        avgLast3 /= count;
        amountVsAvgLast3 = currentAmount / (avgLast3 + 0.01); // Avoid division by zero
    } else {
        amountVsAvgLast3 = 1.0; // Neutral if no history
    }

    // Check if merchant seen before
    merchantSeenBefore = false;
    if (lastTxn1.merchant == currentMerchant ||
        lastTxn2.merchant == currentMerchant ||
        lastTxn3.merchant == currentMerchant) {
        merchantSeenBefore = true;
    }

    // Time since last transaction (simplified - would use actual timestamp parsing)
    timeSinceLastTxn = 0.0; // You'd calculate from timestamps in production
}

// NEW: Add mock keystroke biometrics for testing (simulate normal user typing pattern)
void BiometricSnapshot::populateMockKeystrokeData() {
    // Simulate normal keystroke patterns
    avgDwellTime = 90.0 + (rand() % 30);     // 90-120ms typical for normal users
    stdDwellTime = 15.0 + (rand() % 10);     // 15-25ms std dev
    avgFlightTime = 60.0 + (rand() % 20);    // 60-80ms between keys
    stdFlightTime = 12.0 + (rand() % 8);     // 12-20ms std dev
    typingSpeed = 6.5 + (double)(rand() % 30) / 10.0;  // 6.5-9.5 chars/sec
    backspaceRate = 2.0 + (rand() % 5);      // 2-7% backspace rate
    pauseCount = rand() % 3;                 // 0-2 pauses
}
