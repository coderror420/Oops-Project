#ifndef FRAUDDETECTOR_H
#define FRAUDDETECTOR_H

#include "BiometricSnapshot.h"
#include <vector>
#include <string>

// Model parameters loaded from JSON
struct ModelParams {
    std::string userId;
    std::vector<double> scalerMean;
    std::vector<double> scalerStd;
    std::vector<std::vector<double>> supportVectors;
    std::vector<double> dualCoef;
    double intercept;
    double gamma;
};

class FraudDetector {
private:
    ModelParams model;
    bool modelLoaded;

    // RBF kernel computation
    double rbfKernel(const std::vector<double>& x,
                     const std::vector<double>& y) const;

    // Normalize features
    std::vector<double> normalize(const std::vector<double>& features) const;

public:
    FraudDetector();

    // Load model from JSON file
    bool loadModel(const std::string& modelPath);

    // Detect fraud (returns anomaly score 0.0-1.0)
    double detectFraud(BiometricSnapshot& snapshot);

    bool isModelLoaded() const { return modelLoaded; }
};

#endif
