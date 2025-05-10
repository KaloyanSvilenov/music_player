#ifndef AUDIOQUEUEMANAGER_H
#define AUDIOQUEUEMANAGER_H

#include "audiometadata.h"
#include <player/audioplayer.h>
#include <player/waveformprogressbar.h>
#include <player/playbutton.h>

#include <QObject>
#include <QTableWidget>
#include <QQueue>
#include <QString>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>

class AudioQueueTable : public QObject
{
    Q_OBJECT
public:
    explicit AudioQueueTable(QTableWidget *tableWidget,
                             AudioPlayer *audioPlayer,
                             WaveformProgressBar* bar,
                             PlayButton *button,
                             QListView* metadataView = nullptr,
                             QLabel* coverArtLabel = nullptr,
                             QObject *parent = nullptr);

    void onCustomContextMenuRequested();

    // Queue operations
    void enqueue(const QString& filePath);  // Add to end
    QString dequeue();                      // Remove from front
    QString peekFront() const;              // View front item
    bool isEmpty() const;                   // Check if empty
    void clearQueue();                      // Clear all
    int count() const;                      // Get item count
    void removeSelectedRows();
    void removeRow(int row);
    void moveUp();
    void moveDown();
    void swapTableRows(int, int);
    bool checkExists(const QString &filePath);

    // Iteration
    QString at(int index) const;
    QList<QString> getAllItems() const;

    // Internal queue storage
    QQueue<QString> m_fileQueue;

    void setDisplayWidgets(QListView* metadataView, QLabel* coverArtLabel);

    // Methods for playing the next/previous song
    void nextSong();  // Play the next song
    void previousSong();  // Play the previous song
    void shuffleQueue();
    void setLoopEnabled(bool enabled);
    bool isLoopEnabled() const;  // Getter for loopEnabled flag

signals:
    void queueChanged();
    void rowRemoved(int row, const QString& filePath);
    void rowDoubleClicked(int row, const QString& filePath);
    void songChanged();  // Signal for song change to update the UI

private slots:
    void handleItemDoubleClick(QTableWidgetItem* item);
    void displayMetadata(int row);
    void displayCoverArt(const QString& filePath);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void playSongAtIndex(int indexChange);  // Internal slot for handling the next song

private:
    AudioMetadataReader m_metadataReader;
    QTableWidget *m_tableWidget;
    QMenu *m_contextMenu;
    int currentSongIndex;
    QListView* m_metadataView;
    QLabel* m_coverArtLabel;
    QStandardItemModel* m_metadataModel;
    AudioPlayer* player;
    WaveformProgressBar* progressBar;
    PlayButton* playButton;
    bool loopEnabled = false;
};
#endif // AUDIOQUEUEMANAGER_H
