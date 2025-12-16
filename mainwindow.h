#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private:
    Ui::MainWindow *ui;
    QString userId;
    QString password;
    QString recipientId;
    double amount;
     QString otp;
};
#endif // MAINWINDOW_H
