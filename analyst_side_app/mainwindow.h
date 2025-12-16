#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_generateOtp_clicked();
    void on_submitOtp_clicked();

private:
    Ui::MainWindow *ui;

    //To add
    //Member variables (not global)
    QElapsedTimer otpTimer;
    bool isTimerActive;
};

#endif // MAINWINDOW_H
