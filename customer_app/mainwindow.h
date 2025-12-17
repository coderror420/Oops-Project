#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QElapsedTimer>
#include <QMainWindow>
#include "FraudDetector.h"
#include "BiometricSnapshot.h"
#include "BiometricCSVLogger.h"
#include "transactioncsvlogger.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void goNext();
    void goBack();

    void on_getOtp_clicked();

    void on_verify_clicked();

    void on_next_3_clicked();

private:
    Ui::MainWindow *ui;
    QString userId;
    QString password;
    QString recipientId;
    double amount;
    QString otp;
    QElapsedTimer otpTimer;
    bool isTimerActive;
    FraudDetector* fraudDetector;
    double lastOtpTime;
    int otpAttempts;

    // Helper to get mock history
    TransactionRecord getLastTransaction(int index);
    //Real history
    std::vector<TransactionRecord> getLastThreeTransactions(const QString& userId);
    TransactionRecord parseTransactionLine(const QString& line);
};
#endif // MAINWINDOW_H
