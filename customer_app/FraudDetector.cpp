#include "FraudDetector.h"
#include <fstream>
#include <cmath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

FraudDetector::FraudDetector() : modelLoaded(false) {}

bool FraudDetector::loadModel(const std::string& modelPath) {
    QFile file(QString::fromStdString(modelPath));

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Cannot open model file:" << QString::fromStdString(modelPath);
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Error: Invalid JSON in model file";
        return false;
    }

    QJsonObject json = doc.object();

    // Parse model parameters
    model.userId = json["user_id"].toString().toStdString();
    model.intercept = json["intercept"].toDouble();
    model.gamma = json["gamma"].toDouble();

    // Parse scaler mean
    QJsonArray meanArray = json["scaler_mean"].toArray();
    for (const auto& val : meanArray) {
        model.scalerMean.push_back(val.toDouble());
    }

    // Parse scaler std
    QJsonArray stdArray = json["scaler_std"].toArray();
    for (const auto& val : stdArray) {
        model.scalerStd.push_back(val.toDouble());
    }

    // Parse support vectors
    QJsonArray svArray = json["support_vectors"].toArray();
    for (const auto& svVal : svArray) {
        QJsonArray sv = svVal.toArray();
        std::vector<double> vec;
        for (const auto& v : sv) {
            vec.push_back(v.toDouble());
        }
        model.supportVectors.push_back(vec);
    }

    // Parse dual coefficients
    QJsonArray dualArray = json["dual_coef"].toArray();
    if (!dualArray.isEmpty()) {
        QJsonArray coeffs = dualArray[0].toArray();
        for (const auto& val : coeffs) {
            model.dualCoef.push_back(val.toDouble());
        }
    }

    modelLoaded = true;
    qDebug() << "Model loaded for user:" << QString::fromStdString(model.userId);
    qDebug() << "Support vectors:" << model.supportVectors.size();

    return true;
}

double FraudDetector::detectFraud(BiometricSnapshot& snapshot) {
    if (!modelLoaded) {
        qDebug() << "Error: Model not loaded!";
        return 0.5; // Default neutral score
    }

    // Extract features from snapshot
    std::vector<double> features = snapshot.toFeatureVector();

    // Normalize features
    std::vector<double> normalizedFeatures = normalize(features);

    // Compute decision function (One-Class SVM)
    double decision = model.intercept;

    for (size_t i = 0; i < model.supportVectors.size(); i++) {
        double kernelValue = rbfKernel(normalizedFeatures, model.supportVectors[i]);
        decision += model.dualCoef[i] * kernelValue;
    }

    // Convert to anomaly score
    // One-Class SVM: decision > 0 = normal, decision < 0 = anomaly
    double anomalyScore;
    if (decision > 0) {
        anomalyScore = 0.0; // Normal
    } else {
        // Map negative values to 0-1 score (more negative = higher anomaly)
        anomalyScore = std::min(1.0, std::abs(decision) / 10.0);
    }

    // Update snapshot with result
    snapshot.fraudScore = anomalyScore;
    snapshot.isFlagged = (anomalyScore > 0.7); // Threshold

    if (snapshot.isFlagged) {
        snapshot.flagReason = "Behavioral biometrics anomaly detected";
    }

    qDebug() << "Fraud detection score:" << anomalyScore
             << "Flagged:" << snapshot.isFlagged;

    return anomalyScore;
}

std::vector<double> FraudDetector::normalize(const std::vector<double>& features) const {
    std::vector<double> normalized;

    for (size_t i = 0; i < features.size(); i++) {
        if (i < model.scalerMean.size() && i < model.scalerStd.size()) {
            double norm = (features[i] - model.scalerMean[i]) / (model.scalerStd[i] + 1e-8);
            normalized.push_back(norm);
        } else {
            normalized.push_back(features[i]);
        }
    }

    return normalized;
}

double FraudDetector::rbfKernel(const std::vector<double>& x,
                                const std::vector<double>& y) const {
    double sumSquared = 0.0;
    size_t minSize = std::min(x.size(), y.size());

    for (size_t i = 0; i < minSize; i++) {
        double diff = x[i] - y[i];
        sumSquared += diff * diff;
    }

    return std::exp(-model.gamma * sumSquared);
}
