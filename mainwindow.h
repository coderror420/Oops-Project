#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QElapsedTimer>
#include <QMainWindow>

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
};
#endif // MAINWINDOW_H
