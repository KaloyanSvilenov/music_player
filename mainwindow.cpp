#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->resize(800, ui->tableWidget->width());

    ui->tableWidget->setColumnWidth(0, 30);
    ui->tableWidget->setColumnWidth(1, 150);
    ui->tableWidget->setColumnWidth(2, 200);
    ui->tableWidget->setColumnWidth(3, 150);
    ui->tableWidget->setColumnWidth(4, 70);
    ui->tableWidget->setColumnWidth(5, 80);
    ui->tableWidget->setColumnWidth(6, 120);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
}

MainWindow::~MainWindow()
{
    delete ui;
}
