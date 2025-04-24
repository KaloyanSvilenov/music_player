#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QResizeEvent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize components
    m_queueTable = new AudioQueueTable(ui->tableWidget, this);
    m_treeWindowManager = new TreeWidgetWindow(ui->fileTree, this);

    // Set up context menu
    m_queueTable->onCustomContextMenuRequested();

    // Connections
    connect(m_treeWindowManager, &TreeWidgetWindow::fileAddRequested,
            m_queueTable, &AudioQueueTable::enqueue);

}

MainWindow::~MainWindow()
{
    // Save state before closing
    m_treeWindowManager->saveState();
    delete ui;
}
