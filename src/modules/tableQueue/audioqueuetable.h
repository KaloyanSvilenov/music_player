#ifndef AUDIOQUEUEMANAGER_H
#define AUDIOQUEUEMANAGER_H

#include "audiometadata.h"

#include <QObject>
#include <QTableWidget>
#include <QQueue>
#include <QString>
#include <QLabel>
#include <QListWidget>
#include <QStandardItemModel>

class AudioQueueTable : public QObject
{
    Q_OBJECT
public:
    explicit AudioQueueTable(QTableWidget *tableWidget,
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

signals:
    void queueChanged();
    void rowRemoved(int row, const QString& filePath);
    void rowDoubleClicked(int row, const QString& filePath);

private slots:
    void handleItemDoubleClick(QTableWidgetItem* item);
    void displayMetadata(int row);
    void displayCoverArt(const QString& filePath);

private:
    AudioMetadataReader m_metadataReader;
    QTableWidget *m_tableWidget;
    QMenu *m_contextMenu;

    QListView* m_metadataView;
    QLabel* m_coverArtLabel;
    QStandardItemModel* m_metadataModel;
};
#endif // AUDIOQUEUEMANAGER_H
