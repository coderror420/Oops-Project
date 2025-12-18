#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QDir>
#include <vector>
#include <algorithm>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (ui->tableWidget)
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (ui->tableWidget_2)
        ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    on_comboBox_currentIndexChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::populateTableFromCSV(const QString &filePath, QTableWidget *tableWidget, bool filterFlaggedOnly) {
    if (!tableWidget) return;

    tableWidget->clear();
    tableWidget->setRowCount(0);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        tableWidget->setColumnCount(1);
        tableWidget->setHorizontalHeaderLabels({"File Not Found"});
        return;
    }

    QTextStream in(&file);
    if (in.atEnd()) return;

    QString headerLine = in.readLine();
    QStringList headers = headerLine.split(",");

    int flagColumnIndex = -1;
    if (filterFlaggedOnly) {
        for (int i = 0; i < headers.size(); ++i) {
            QString h = headers[i].toLower();
            if (h.contains("is_flagged")) {
                flagColumnIndex = i;
                std::cout<<"im here";
                break;
            }
        }
    }

    std::vector<QStringList> allRows;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;

        QStringList fields = line.split(",");
        if (fields.size() != headers.size()) continue;

        if (filterFlaggedOnly && flagColumnIndex != -1) {
            QString val = fields[flagColumnIndex].toLower().trimmed();
            bool isFlagged = (val == "1" || val == "true" || val == "yes" || val == "is_flagged");
            if (!isFlagged) continue;
            std::cout<<"im here";
        }

        allRows.push_back(fields);
    }
    file.close();

    if (allRows.empty()) {
        tableWidget->setColumnCount(headers.size());
        tableWidget->setHorizontalHeaderLabels(headers);
        return;
    }

    int rowsToDisplay = std::min((int)allRows.size(), 10);

    tableWidget->setColumnCount(headers.size());
    tableWidget->setRowCount(rowsToDisplay);
    tableWidget->setHorizontalHeaderLabels(headers);

    int tableRow = 0;
    int startIndex = (int)allRows.size() - 1;
    int endIndex = (int)allRows.size() - rowsToDisplay;

    for (int i = startIndex; i >= endIndex; --i) {
        if (i < 0 || i >= (int)allRows.size()) break;

        const QStringList& rowData = allRows[i];
        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(tableRow, col, item);
        }
        tableRow++;
    }
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    for(int j=0;j<tableWidget->columnCount();j++){
        tableWidget->setColumnWidth(j, 80);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QString basePath = "../../../customer_app/build/Desktop_Qt_6_10_0_MinGW_64_bit-Debug/data/";
    QString bioPath = basePath + "biometric_data.csv";
    QString transPath = basePath + "transaction_data.csv";

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget_2->clear();
    ui->tableWidget_2->setRowCount(0);

    if(index == 0) {
        populateTableFromCSV(transPath, ui->tableWidget, true);
        populateTableFromCSV(bioPath, ui->tableWidget_2, true);
    }
    else if(index == 1) {
        populateTableFromCSV(bioPath, ui->tableWidget_2, false);
    }
    else if(index == 2) {
        populateTableFromCSV(transPath, ui->tableWidget, false);
    }
}
