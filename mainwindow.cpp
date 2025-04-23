#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_queueTable = new AudioQueueTable(ui->tableWidget, this);
    m_queueTable->onCustomContextMenuRequested();

    m_treeWindowManager = new TreeWidgetWindow(ui->fileTree, this);
    connect(m_treeWindowManager, &TreeWidgetWindow::fileAddRequested,
            m_queueTable, &AudioQueueTable::enqueue);
}

MainWindow::~MainWindow()
{
    delete ui;
}
