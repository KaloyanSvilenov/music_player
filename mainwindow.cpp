#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QResizeEvent>
#include <QMenu>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Table Setup
    ui->tableWidget->resize(800, ui->tableWidget->width());
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 150);
    ui->tableWidget->setColumnWidth(3, 70);
    ui->tableWidget->setColumnWidth(4, 80);
    ui->tableWidget->setColumnWidth(5, 80);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    //Button Images
    ui->filterButton->setIconSize(QSize(25, 25));
    ui->loopButton->setIconSize(QSize(40, 40));
    ui->shuffleButton->setIconSize(QSize(25, 25));
    ui->pausePlay->setIconSize(QSize(50,50));
    ui->backButton->setIconSize(QSize(40,40));
    ui->nextButton->setIconSize(QSize(40,40));
    ui->nextButton->setDefaultIcon("./images/next.png");
    ui->nextButton->setHoverIcon("./images/next hover.png");
    ui->backButton->setDefaultIcon("./images/back.png");
    ui->backButton->setHoverIcon("./images/back hover.png");
    ui->shuffleButton->setDefaultIcon("./images/shuffle.png");
    ui->shuffleButton->setHoverIcon("./images/shuffle hover.png");
    ui->loopButton->setDefaultIcon("./images/loop.png");
    ui->loopButton->setHoverIcon("./images/loop hover.png");
    ui->filterButton->setDefaultIcon("./images/filter.png");
    ui->filterButton->setHoverIcon("./images/filter hover.png");
    ui->pausePlay->setDefaultPlayIcon("./images/play.png");
    ui->pausePlay->setHoverPlayIcon("./images/play hover.png");
    ui->pausePlay->setDefaultPauseIcon("./images/pause.png");
    ui->pausePlay->setHoverPauseIcon("./images/pause hover.png");

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

