#ifndef BIOMETRICCSVLOGGER_H
#define BIOMETRICCSVLOGGER_H

#include "BiometricSnapshot.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QString>
#include <fstream>
#include <string>

class BiometricCSVLogger {
public:
    static bool saveSnapshot(const BiometricSnapshot& snapshot,
                             const std::string& filename = "data/biometric_data.csv");

    static bool fileExists(const std::string& filename);

private:
    static void writeHeader(std::ofstream& file);
    static void writeDataRow(std::ofstream& file, const BiometricSnapshot& snapshot);
};

#endif
