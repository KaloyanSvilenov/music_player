#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QResizeEvent>
#include <QTimer>
#include <QMenu>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize components
    m_queueTable = new AudioQueueTable(ui->tableWidget,
                                       ui->listViewMetaData,  // Your QListView
                                       ui->label,            // Your QLabel for cover art
                                       this);
    m_treeWindowManager = new TreeWidgetWindow(ui->fileTree, this);

    // Set up context menu
    m_queueTable->onCustomContextMenuRequested();

    // Connections
    connect(m_treeWindowManager, &TreeWidgetWindow::fileAddRequested,
            m_queueTable, &AudioQueueTable::enqueue);

    // m_queueTable->setDisplayWidgets(ui->listViewMetaData, ui->label);
      
    //Button Icons
    ui->filterButton->setIconSize(QSize(25, 25));
    ui->loopButton->setIconSize(QSize(40, 40));
    ui->shuffleButton->setIconSize(QSize(25, 25));
    ui->pausePlay->setIconSize(QSize(50,50));
    ui->backButton->setIconSize(QSize(40,40));
    ui->nextButton->setIconSize(QSize(40,40));
    ui->nextButton->setDefaultIcon(":resources/icons/next.png");
    ui->nextButton->setHoverIcon(":resources/icons/next_hover.png");
    ui->backButton->setDefaultIcon(":resources/icons/back.png");
    ui->backButton->setHoverIcon(":resources/icons/back_hover.png");
    ui->shuffleButton->setDefaultIcon(":resources/icons/shuffle.png");
    ui->shuffleButton->setHoverIcon(":resources/icons/shuffle_hover.png");
    ui->loopButton->setDefaultIcon(":resources/icons/loop.png");
    ui->loopButton->setHoverIcon("::resources/icons/loop_hover.png");
    ui->filterButton->setDefaultIcon(":resources/icons/filter.png");
    ui->filterButton->setHoverIcon(":resources/icons/filter_hover.png");
    ui->pausePlay->setDefaultPlayIcon(":resources/icons/play.png");
    ui->pausePlay->setHoverPlayIcon(":resources/icons/play_hover.png");
    ui->pausePlay->setDefaultPauseIcon(":resources/icons/pause.png");
    ui->pausePlay->setHoverPauseIcon(":resources/icons/pause_hover.png");

    //Filter Button dropdown Menu
    QMenu *menu = new QMenu(this);\
    menu->addAction("File Names");
    menu->addAction("Album");
    menu->addAction("Artist");
    menu->addAction("Song Name");
    menu->addAction("Year");
    ui->filterButton->setMenu(menu);
    connect(menu, &QMenu::triggered, this, &MainWindow::onMenuActionTriggered);

    //TODO: Waveform progress bar logic
}

void MainWindow::onMenuActionTriggered(QAction *action) {
    QString selectedText = action->text(); // Text to be used later with actual logic
}

MainWindow::~MainWindow()
{
    // Save state before closing
    m_treeWindowManager->saveState();
    delete ui;
}

void MainWindow::on_pausePlay_clicked()
{
    ui->pausePlay->changePlaying();
}


void MainWindow::on_loopButton_clicked()
{
    ui->loopButton->changeHoverEnabled();
}

