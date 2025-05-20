#include "audioqueuetable.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QFileInfo>
#include <QHeaderView>
#include <QMimeData>
#include <QMessageBox>
#include <QTimer>
#include <algorithm>  // For std::shuffle
#include <random>     // For random engine


AudioQueueTable::AudioQueueTable(QTableWidget *tableWidget,
                                 AudioPlayer* audioPlayer,
                                 WaveformProgressBar* bar,
                                 PlayButton* button,
                                 QListView* metadataView,
                                 QLabel* coverArtLabel,
                                 QObject *parent)
    : QObject(parent),
    m_tableWidget(tableWidget),
    currentSongIndex(-1),
    m_metadataView(metadataView),
    m_coverArtLabel(coverArtLabel),
    m_metadataModel(new QStandardItemModel(this)),
    player(audioPlayer),
    progressBar(bar),
    playButton(button)
{
    m_tableWidget->resize(800, m_tableWidget->width());
    m_tableWidget->setColumnWidth(0, 150);
    m_tableWidget->setColumnWidth(1, 200);
    m_tableWidget->setColumnWidth(2, 150);
    m_tableWidget->setColumnWidth(3, 70);
    m_tableWidget->setColumnWidth(4, 80);
    m_tableWidget->setColumnWidth(5, 80);
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setHorizontalHeaderLabels({"Name", "Artist", "Album", "Genre", "Duration"});
    m_tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableWidget->verticalHeader()->setStretchLastSection(false);
    m_tableWidget->verticalHeader()->setVisible(false); // temp solution to a style problem
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Metadata view setup
    if (m_metadataView) {
        m_metadataView->setModel(m_metadataModel);
        m_metadataView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    // Connect double-click signal
    connect(m_tableWidget, &QTableWidget::itemDoubleClicked,
            this, &AudioQueueTable::handleItemDoubleClick);

    // Connect for end of song signal
    connect(player->getPlayer(), &QMediaPlayer::mediaStatusChanged,
            this, &AudioQueueTable::onMediaStatusChanged);
}

void AudioQueueTable::setDisplayWidgets(QListView* metadataView, QLabel* coverArtLabel)
{
    m_metadataView = metadataView;
    m_coverArtLabel = coverArtLabel;

    if (m_metadataView) {
        m_metadataView->setModel(m_metadataModel);
        m_metadataView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void AudioQueueTable::handleItemDoubleClick(QTableWidgetItem* item)
{
    int row = item->row();
    QString filePath = m_fileQueue.at(row);
    currentSongIndex = row;

    progressBar->setAudioFile(filePath);

    // Directly play the song without waiting for waveform
    pendingFilePath = filePath;
    player->setCurrentSong(filePath);
    player->play();
    playButton->setToPlaying();

    // Update display immediately
    displayMetadata(row);
    displayCoverArt(filePath);

    emit rowDoubleClicked(row, filePath);
}


void AudioQueueTable::displayMetadata(int row)
{
    if (!m_metadataView) return;

    m_metadataModel->clear();

    QString filePath = m_fileQueue.at(row);
    AudioMetadata meta = m_metadataReader.readMetadata(filePath);

    QStandardItem* pathItem = new QStandardItem("File: " + filePath);
    m_metadataModel->appendRow(pathItem);

    QStandardItem* titleItem = new QStandardItem("Title: " + meta.title);
    m_metadataModel->appendRow(titleItem);

    QStandardItem* artistItem = new QStandardItem("Artist: " + meta.artist);
    m_metadataModel->appendRow(artistItem);

    QStandardItem* albumItem = new QStandardItem("Album: " + meta.album);
    m_metadataModel->appendRow(albumItem);

    QStandardItem* genreItem = new QStandardItem("Genre: " + meta.genre);
    m_metadataModel->appendRow(genreItem);

    QStandardItem* durationItem = new QStandardItem("Duration: " + meta.duration);
    m_metadataModel->appendRow(durationItem);

    QStandardItem* bitrateItem = new QStandardItem("Bitrate: " + meta.bitrate + " kbps");
    m_metadataModel->appendRow(bitrateItem);

}

void AudioQueueTable::displayCoverArt(const QString& filePath)
{
    if (!m_coverArtLabel) return;

    // Clear previous content
    m_coverArtLabel->clear();

    QImage cover = m_metadataReader.extractCoverArt(filePath);
    qDebug() << "Cover art valid:" << !cover.isNull();
    if (!cover.isNull()) {
        // Convert to pixmap and scale properly
        QPixmap pixmap = QPixmap::fromImage(cover);

        // Calculate scaled size maintaining aspect ratio
        int labelWidth = m_coverArtLabel->width() - 2; // Padding
        int labelHeight = m_coverArtLabel->height() - 2;
        pixmap = pixmap.scaled(labelWidth, labelHeight,
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);

        m_coverArtLabel->setPixmap(pixmap);
    } else {
        // Show placeholder text if no art found
        m_coverArtLabel->setText("No Cover Art");
        m_coverArtLabel->setStyleSheet("QLabel { color: gray; font-style: italic; }");
        m_coverArtLabel->setAlignment(Qt::AlignCenter);
    }
}

void AudioQueueTable::onCustomContextMenuRequested()
{
    // Create context menu
    m_contextMenu = new QMenu(m_tableWidget);
    QAction *moveUpAction = m_contextMenu->addAction("Move up");
    QAction *moveDownAction = m_contextMenu->addAction("Move down");
    QAction *removeAction = m_contextMenu->addAction("Remove song");
    QAction *clearAction = m_contextMenu->addAction("Clear queue");

    // Connect actions
    connect(moveUpAction, &QAction::triggered, this, &AudioQueueTable::moveUp);
    connect(moveDownAction, &QAction::triggered, this, &AudioQueueTable::moveDown);
    connect(removeAction, &QAction::triggered, this, &AudioQueueTable::removeSelectedRows);
    connect(clearAction, &QAction::triggered, this, &AudioQueueTable::clearQueue);

    // Context menu request handler
    connect(m_tableWidget, &QTableWidget::customContextMenuRequested, this,
            [this, moveUpAction, moveDownAction, removeAction](const QPoint &pos) {
                bool hasSelection = m_tableWidget->selectedItems().count() > 0;
                bool singleSelection = m_tableWidget->selectedRanges().count() == 1;

                // Enable/disable actions based on selection state
                moveUpAction->setEnabled(hasSelection && singleSelection);
                moveDownAction->setEnabled(hasSelection && singleSelection);
                removeAction->setEnabled(hasSelection);

                m_contextMenu->exec(m_tableWidget->viewport()->mapToGlobal(pos));
            });
}

bool AudioQueueTable::checkExists(const QString &filePath)
{
    // Check if file already exists in queue
    int existingIndex = m_fileQueue.indexOf(filePath);
    if (existingIndex != -1) {
        // Highlight the existing row briefly
        m_tableWidget->selectRow(existingIndex);
        QTimer::singleShot(300, this, [this]() {
            m_tableWidget->clearSelection();
        });

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(m_tableWidget,
                                      "Duplicate Entry",
                                      "This song is already in the queue!\nAdd again?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return false;
        }
    }

    return true;
}

void AudioQueueTable::enqueue(const QString& filePath) {

    if (!this->checkExists(filePath)){
        return;
    }

    // Add to internal queue
    m_fileQueue.enqueue(filePath);

    // Add to table UI
    AudioMetadata meta = m_metadataReader.readMetadata(filePath);
    const int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);

    // Name column
    QTableWidgetItem *nameItem = new QTableWidgetItem(meta.title.isEmpty() ?
                                                          QFileInfo(filePath).fileName() : meta.title);
    nameItem->setData(Qt::UserRole, filePath);
    m_tableWidget->setItem(row, 0, nameItem);

    // Artist column
    QTableWidgetItem *artist = new QTableWidgetItem(meta.artist);
    m_tableWidget->setItem(row, 1, artist);

    // Album column
    QTableWidgetItem *album = new QTableWidgetItem(meta.album);
    m_tableWidget->setItem(row, 2, album);

    // Genre column
    QTableWidgetItem *genre = new QTableWidgetItem(meta.genre);
    m_tableWidget->setItem(row, 3, genre);

    // Duration column
    QTableWidgetItem *durationItem = new QTableWidgetItem(meta.duration);
    durationItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_tableWidget->setItem(row, 4, durationItem);

    m_tableWidget->update();

    emit queueChanged();
}

QString AudioQueueTable::dequeue() {
    if (m_fileQueue.isEmpty())
        return QString();

    // Remove from internal queue
    QString filePath = m_fileQueue.dequeue();

    // Remove from table UI (first row)
    if (m_tableWidget->rowCount() > 0) {
        m_tableWidget->removeRow(0);
    }

    return filePath;
}

QString AudioQueueTable::peekFront() const {
    return m_fileQueue.isEmpty() ? QString() : m_fileQueue.head();
}

bool AudioQueueTable::isEmpty() const {
    return m_fileQueue.isEmpty();
}

void AudioQueueTable::clearQueue() {
    m_fileQueue.clear();
    m_tableWidget->setRowCount(0);
}

int AudioQueueTable::count() const {
    return m_fileQueue.size();
}

// Iteration methods
QString AudioQueueTable::at(int index) const {
    return (index >= 0 && index < m_fileQueue.size()) ? m_fileQueue.at(index) : QString();
}

QList<QString> AudioQueueTable::getAllItems() const {
    return m_fileQueue.toList();
}

void AudioQueueTable::removeSelectedRows()
{
    // Get selected rows (using selectedRanges for better accuracy)
    QList<QTableWidgetSelectionRange> ranges = m_tableWidget->selectedRanges();
    QList<int> rows;

    foreach(const QTableWidgetSelectionRange &range, ranges) {
        for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
            rows.append(row);
        }
    }

    // Remove duplicates and sort in reverse order
    rows = QSet<int>(rows.begin(), rows.end()).values();
    std::sort(rows.begin(), rows.end(), std::greater<int>());

    qDebug() << "Attempting to remove rows:" << rows;

    // Remove items
    foreach(int row, rows) {
        if (row >= 0 && row < m_fileQueue.size() && row < m_tableWidget->rowCount()) {
            QString removedFile = m_fileQueue.at(row);
            qDebug() << "Removing row" << row << "with file:" << removedFile;

            // Remove from data structure
            m_fileQueue.removeAt(row);

            // Remove from UI
            m_tableWidget->removeRow(row);

            emit rowRemoved(row, removedFile);
        }
    }

    emit queueChanged();
    qDebug() << "Queue after removal. Count:" << m_fileQueue.size();
}

void AudioQueueTable::moveUp()
{
    QList<QTableWidgetSelectionRange> ranges = m_tableWidget->selectedRanges();
    if (ranges.isEmpty()) return;

    int selectedRow = ranges.first().topRow();
    if (selectedRow <= 0) return; // Can't move top item up

    // Move in data structure
    m_fileQueue.swapItemsAt(selectedRow, selectedRow - 1);

    // Move in UI
    swapTableRows(selectedRow, selectedRow - 1);

    // Reselect the moved item
    m_tableWidget->selectRow(selectedRow - 1);
    emit queueChanged();
}

void AudioQueueTable::moveDown()
{
    QList<QTableWidgetSelectionRange> ranges = m_tableWidget->selectedRanges();
    if (ranges.isEmpty()) return;

    int selectedRow = ranges.first().topRow();
    if (selectedRow >= m_fileQueue.size() - 1) return; // Can't move bottom item down

    // Move in data structure
    m_fileQueue.swapItemsAt(selectedRow, selectedRow + 1);

    // Move in UI
    swapTableRows(selectedRow, selectedRow + 1);

    // Reselect the moved item
    m_tableWidget->selectRow(selectedRow + 1);
    emit queueChanged();
}

void AudioQueueTable::swapTableRows(int row1, int row2)
{
    for (int col = 0; col < m_tableWidget->columnCount(); ++col) {
        QTableWidgetItem* item1 = m_tableWidget->takeItem(row1, col);
        QTableWidgetItem* item2 = m_tableWidget->takeItem(row2, col);

        m_tableWidget->setItem(row1, col, item2);
        m_tableWidget->setItem(row2, col, item1);
    }
}

void AudioQueueTable::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        if (loopEnabled) {
            // Restart the current song using the path instead of index
            int index = m_fileQueue.indexOf(pendingFilePath);
            if (index != -1) {
                currentSongIndex = index;
                QString currentSong = m_fileQueue.at(currentSongIndex);
                player->setCurrentSong(currentSong);
                progressBar->setValue(0);  // Reset progress bar
                playButton->setToPlaying();
                player->play();
            }
        } else {
            nextSong();
        }
    }
}


void AudioQueueTable::playSongAtIndex(int indexChange)
{
    if (isEmpty()) {
        qDebug() << "Queue is empty. No more songs to play.";
        return;
    }

    currentSongIndex += indexChange;
    if (currentSongIndex >= m_fileQueue.size()) {
        currentSongIndex = 0;
    } else if (currentSongIndex < 0) {
        currentSongIndex = m_fileQueue.size() - 1;
    }

    QString song = m_fileQueue.at(currentSongIndex);

    progressBar->setAudioFile(song);

    // Directly play without waveform dependency
    pendingFilePath = song;
    player->setCurrentSong(song);
    player->play();
    playButton->setToPlaying();

    // Update display
    displayMetadata(currentSongIndex);
    displayCoverArt(song);
}

void AudioQueueTable::nextSong()
{
    playSongAtIndex(1);  // Move forward by 1
}

void AudioQueueTable::previousSong()
{
    playSongAtIndex(-1);  // Move backward by 1
}

void AudioQueueTable::shuffleQueue()
{
    if (m_fileQueue.isEmpty()) {
        return;
    }

    // Shuffle the queue using std::shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_fileQueue.begin(), m_fileQueue.end(), g);

    // Now update the UI (table) to reflect the shuffled order
    for (int row = 0; row < m_fileQueue.size(); ++row) {
        QString filePath = m_fileQueue.at(row);
        AudioMetadata meta = m_metadataReader.readMetadata(filePath);

        // Name column
        QTableWidgetItem *nameItem = new QTableWidgetItem(meta.title.isEmpty() ?
                                                              QFileInfo(filePath).fileName() : meta.title);
        nameItem->setData(Qt::UserRole, filePath);
        m_tableWidget->setItem(row, 0, nameItem);

        // Artist column
        QTableWidgetItem *artist = new QTableWidgetItem(meta.artist);
        m_tableWidget->setItem(row, 1, artist);

        // Album column
        QTableWidgetItem *album = new QTableWidgetItem(meta.album);
        m_tableWidget->setItem(row, 2, album);

        // Genre column
        QTableWidgetItem *genre = new QTableWidgetItem(meta.genre);
        m_tableWidget->setItem(row, 3, genre);

        // Duration column
        QTableWidgetItem *durationItem = new QTableWidgetItem(meta.duration);
        durationItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_tableWidget->setItem(row, 4, durationItem);
    }

    // Reset currentSongIndex to the start (index 0) after shuffle
    currentSongIndex = -1;

    emit queueChanged();  // Notify that the queue has changed
}

void AudioQueueTable::setLoopEnabled(bool enabled) {
    loopEnabled = enabled;
}

bool AudioQueueTable::isLoopEnabled() const {
    return loopEnabled;
}


