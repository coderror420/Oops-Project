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

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (ui->tableWidget)
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    if (ui->tableWidget_2)
        ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    on_comboBox_currentIndexChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());
}

void MainWindow::loadGraphInView(const QString &filePath, QGraphicsView *view) {
    if (!view) return;

    if (view->scene()) {
        delete view->scene();
    }

    QGraphicsScene *scene = new QGraphicsScene(this);
    QPixmap pixmap(filePath);

    if (!pixmap.isNull()) {
        scene->addPixmap(pixmap);
        view->setScene(scene);
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    } else {
        scene->addText("Graph not found:\n" + filePath);
        view->setScene(scene);
        qDebug() << "Failed to load graph image at:" << filePath;
    }
}

void MainWindow::populateTableFromCSV(const QString &filePath, QTableWidget *tableWidget, bool filterFlaggedOnly) {
    if (!tableWidget) return;

    tableWidget->clear();
    tableWidget->setRowCount(0);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        tableWidget->setColumnCount(1);
        tableWidget->setHorizontalHeaderLabels({"Error"});
        tableWidget->setRowCount(1);
        tableWidget->setItem(0, 0, new QTableWidgetItem("File Not Found: " + filePath));
        return;
    }

    QTextStream in(&file);
    if (in.atEnd()) {
        file.close();
        return;
    }

    QString headerLine = in.readLine();
    QStringList headers = headerLine.split(",");

    for(auto &h : headers) {
        h = h.trimmed().remove('"');
    }

    int flagColumnIndex = -1;
    if (filterFlaggedOnly) {
        for (int i = 0; i < headers.size(); ++i) {
            QString h = headers[i].toLower().trimmed();
            if (h == "is_flagged" || h == "flag" || h == "fraud") {
                flagColumnIndex = i;
                qDebug() << "Found flag column at index:" << i << "Header:" << headers[i];
                break;
            }
        }
    }

    std::vector<QStringList> allRows;
    int lineNumber = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;
        if (line.trimmed().isEmpty()) continue;

        QStringList fields = line.split(",");
        for(auto &f : fields) {
            f = f.trimmed().remove('"');
        }

        if (fields.size() != headers.size()) {
            qDebug() << "Line" << lineNumber << "has" << fields.size() << "fields, expected" << headers.size();
            continue;
        }

        if (filterFlaggedOnly && flagColumnIndex != -1) {
            if (flagColumnIndex >= fields.size()) {
                qDebug() << "Flag column index out of range for line" << lineNumber;
                continue;
            }

            QString val = fields[flagColumnIndex].trimmed();
            qDebug() << "Line" << lineNumber << "flag value:" << val;

            if (val == "1") {
                allRows.push_back(fields);
                qDebug() << "Added flagged row" << allRows.size();
            }
        } else {
            allRows.push_back(fields);
        }
    }
    file.close();

    qDebug() << "Total rows after filtering:" << allRows.size();

    if (allRows.empty()) {
        tableWidget->setColumnCount(headers.size() > 0 ? headers.size() : 1);
        tableWidget->setHorizontalHeaderLabels(headers);
        tableWidget->setRowCount(1);

        QTableWidgetItem *msgItem;
        if (filterFlaggedOnly && flagColumnIndex != -1) {
            msgItem = new QTableWidgetItem("No Suspicious Activity Detected (Clean)");
        } else {
            msgItem = new QTableWidgetItem("No Data Found");
        }
        tableWidget->setItem(0, 0, msgItem);
        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
    for(int j=0; j<tableWidget->columnCount(); j++){
        tableWidget->setColumnWidth(j, 120);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QString basePath = "./data/";

    QString bioPath = basePath + "biometric_data.csv";
    QString transPath = basePath + "transaction_data.csv";
    QString graph1Path = basePath + "graph1.png";
    QString graph2Path = basePath + "graph2.png";

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget_2->clear();
    ui->tableWidget_2->setRowCount(0);

    qDebug() << "Loading data for index:" << index;

    if(index == 0) {
        qDebug() << "Dashboard View: Loading flagged transactions";
        populateTableFromCSV(transPath, ui->tableWidget, true);
        qDebug() << "Dashboard View: Loading flagged biometric";
        populateTableFromCSV(bioPath, ui->tableWidget_2, true);
    }
    else if(index == 1) {
        qDebug() << "Biometric View: Loading all biometric history";
        populateTableFromCSV(bioPath, ui->tableWidget_2, false);
    }
    else if(index == 2) {
        qDebug() << "Transaction View: Loading all transaction history";
        populateTableFromCSV(transPath, ui->tableWidget, false);
    }

    loadGraphInView(graph1Path, ui->graphicsView);
    loadGraphInView(graph2Path, ui->graphicsView_2);
}
