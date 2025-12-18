#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QTableWidget> // Required for passing table widgets

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
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    void loadGraphInView(const QString &filePath, QGraphicsView *view);

    // Helper function to handle CSV reading and table population
    void populateTableFromCSV(const QString &filePath, QTableWidget *tableWidget, bool filterFlaggedOnly);
};

#endif // MAINWINDOW_H
