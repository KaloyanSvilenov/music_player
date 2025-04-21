#ifndef AUDIOQUEUEMANAGER_H
#define AUDIOQUEUEMANAGER_H

#include "audiometadata.h"

#include <QObject>
#include <QTableWidget>

class AudioQueueManager : public QObject
{
    Q_OBJECT
public:
    explicit AudioQueueManager(QTableWidget *tableWidget, QObject *parent = nullptr);

    void setupConnections();
    void addToQueue(const QString &filePath);

public slots:
    void removeSelectedRows();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QTableWidget *m_tableWidget;
    QMenu *m_contextMenu;
    void setupTable();

    AudioMetadataReader m_metadataReader;
};
#endif // AUDIOQUEUEMANAGER_H
