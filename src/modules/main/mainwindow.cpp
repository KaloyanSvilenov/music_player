#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QResizeEvent>
#include <QTimer>
#include <QMenu>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QStandardPaths>
#include <player/audioplayer.h>

QString formatTime(qint64 ms);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    AudioPlayer *audioPlayer = new AudioPlayer(this);
    // Initialize components
    m_queueTable = new AudioQueueTable(ui->tableWidget,
                                       audioPlayer,
                                       ui->progressBar,
                                       ui->pausePlay,
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
    ui->nextButton->setDefaultIcon(":/icons/next.png");
    ui->nextButton->setHoverIcon(":/icons/next_hover.png");
    ui->backButton->setDefaultIcon(":/icons/back.png");
    ui->backButton->setHoverIcon(":/icons/back_hover.png");
    ui->shuffleButton->setDefaultIcon(":/icons/shuffle.png");
    ui->shuffleButton->setHoverIcon(":/icons/shuffle_hover.png");
    ui->loopButton->setDefaultIcon(":/icons/loop.png");
    ui->loopButton->setHoverIcon(":/icons/loop_hover.png");
    ui->filterButton->setDefaultIcon(":/icons/filter.png");
    ui->filterButton->setHoverIcon(":/icons/filter_hover.png");
    ui->pausePlay->setDefaultPlayIcon(":/icons/play.png");
    ui->pausePlay->setHoverPlayIcon(":/icons/play_hover.png");
    ui->pausePlay->setDefaultPauseIcon(":/icons/pause.png");
    ui->pausePlay->setHoverPauseIcon(":/icons/pause_hover.png");

    //Filter Button dropdown Menu
    QMenu *menu = new QMenu(this);\
    menu->addAction("File Names");
    menu->addAction("Album");
    menu->addAction("Artist");
    menu->addAction("Song Name");
    //menu->addAction("Year"); // Not supported yet
    ui->filterButton->setMenu(menu);
    connect(menu, &QMenu::triggered, ui->searchBox, &SearchBox::changeTextFromMenu);

    //Audio Player

    connect(ui->pausePlay, &QPushButton::clicked, this, [=]() {
        audioPlayer->togglePlayPause();
    });

    audioPlayer->setVolume(ui->volumeSlider->value() / 100);

    //Volume Control
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [=](int value) {
        audioPlayer->setVolume(value / 100.0);
    });

    connect(audioPlayer->getPlayer(), &QMediaPlayer::positionChanged, this, [=](qint64 pos) {
        ui->progressBar->setValue(static_cast<int>(pos));
        ui->currentTime->setText(formatTime(pos));
    });

    connect(audioPlayer->getPlayer(), &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        ui->progressBar->setMaximum(static_cast<int>(duration));
        ui->totalTime->setText("/ " + formatTime(duration));
    });

    connect(ui->progressBar, &WaveformProgressBar::userSeeked, this, [=](int position) {
        audioPlayer->getPlayer()->setPosition(position);
    });

    connect(audioPlayer->getPlayer(), &QMediaPlayer::positionChanged, ui->progressBar, &QProgressBar::setValue);

    connect(audioPlayer->getPlayer(), &QMediaPlayer::durationChanged, ui->progressBar, &QProgressBar::setMaximum);


    connect(ui->searchBox, &SearchBox::searchTriggered,
            m_treeWindowManager, &TreeWidgetWindow::searchFiles);

    // Add a "Clear Search" button or action
    QAction *clearSearch = new QAction("Clear Search", this);
    clearSearch->setShortcut(QKeySequence("Esc"));
    connect(clearSearch, &QAction::triggered, this, [this]() {
        ui->searchBox->clear();
        m_treeWindowManager->restoreState(); // Reload the original view
    });
    this->addAction(clearSearch);

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
    // Update loop state
    m_queueTable->setLoopEnabled(!m_queueTable->isLoopEnabled());
}

QString formatTime(qint64 ms) {
    int seconds = ms / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void MainWindow::on_backButton_clicked()
{
    m_queueTable->previousSong(); // Call previousSong() from AudioQueueTable
}

void MainWindow::on_nextButton_clicked()
{
    m_queueTable->nextSong(); // Call nextSong() from AudioQueueTable
}

void MainWindow::on_shuffleButton_clicked()
{
    // Shuffle the queue and update the table
    m_queueTable->shuffleQueue();
}

