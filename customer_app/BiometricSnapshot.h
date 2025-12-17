#ifndef BIOMETRICSNAPSHOT_H
#define BIOMETRICSNAPSHOT_H

#include <string>
#include <vector>
#include <ctime>

// Single transaction record
struct TransactionRecord {
    std::string transactionId;
    std::string userId;        // ADD THIS if missing
    double amount;
    std::string merchant;
    std::string category;
    std::string timestamp;
    int hourOfDay;

    TransactionRecord() : transactionId(""), userId(""), amount(0.0),
        merchant(""), category(""),
        timestamp(""), hourOfDay(0) {}
};

// Complete behavioral biometrics snapshot
// This is what gets saved to CSV and used for ML prediction
class BiometricSnapshot {
public:
    // User identification
    std::string userId;
    std::string sessionId;
    time_t timestamp;

    // === KEYSTROKE BIOMETRICS ===
    double avgDwellTime;      // Average key hold time (ms)
    double stdDwellTime;      // Std deviation of dwell times
    double avgFlightTime;     // Average time between keys (ms)
    double stdFlightTime;     // Std deviation of flight times
    double typingSpeed;       // Characters per second
    double backspaceRate;     // Percentage of backspaces
    int pauseCount;          // Number of pauses > 1 sec

    // === OTP BIOMETRICS ===
    double otpResponseTime;   // Time to enter OTP (seconds)
    int otpAttemptsCount;    // Number of OTP attempts
    int totalOtpsSent;       // Total OTPs sent in session

    // === CURRENT TRANSACTION ===
    double currentAmount;
    std::string currentMerchant;
    std::string currentCategory;
    int currentHourOfDay;
    // bool isWeekend;

    // === LAST 3 TRANSACTIONS (Historical Context) ===
    TransactionRecord lastTxn1;
    TransactionRecord lastTxn2;
    TransactionRecord lastTxn3;

    // === DERIVED FEATURES ===
    double amountVsAvgLast3;     // Current amount / avg of last 3
    double timeSinceLastTxn;     // Hours since last transaction
    bool merchantSeenBefore;     // Is this a known merchant?

    // === DEVICE & LOCATION ===
    std::string deviceId;
    bool isKnownDevice;
    double geoDistanceKm;        // Distance from usual location

    // === FRAUD DETECTION RESULT ===
    double fraudScore;           // 0.0 - 1.0 (filled by ML model)
    bool isFlagged;
    std::string flagReason;

    // Constructor
    BiometricSnapshot();

    // Convert to feature vector for ML (14 core features)
    std::vector<double> toFeatureVector() const;

    // Get feature names (for CSV header)
    static std::vector<std::string> getFeatureNames();

    // Helper: Calculate derived features
    void calculateDerivedFeatures();

    // NEW: Populate mock keystroke data for testing
    void populateMockKeystrokeData();
};

#endif // BIOMETRICSNAPSHOT_H
