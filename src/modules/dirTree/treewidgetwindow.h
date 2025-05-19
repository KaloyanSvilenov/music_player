#ifndef TREEWIDGETWINDOW_H
#define TREEWIDGETWINDOW_H

#include <QObject>
#include <QTreeWidget>
#include <QDir>
#include <QSettings>
#include <QCache>
#include "tableQueue/audiometadata.h"

class TreeWidgetWindow : public QObject
{
    Q_OBJECT
public:
    explicit TreeWidgetWindow(QTreeWidget *treeWidget, QObject *parent = nullptr);

    const QStringList audioExtensions = {"*.mp3", "*.wav", "*.flac"};

    void saveState();
    void restoreState();
    void saveExpandedState(QTreeWidgetItem* item);
    void restoreExpandedState(QTreeWidgetItem* item);

signals:
    void fileAddRequested(const QString &filePath);
    void directoryAdded();

public slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void addDirectoryToTree(const QPoint &pos);
    void scanDirectory(QTreeWidgetItem *parentItem, const QString &path);
    void searchFiles(const QString &filter, const QString &searchText);

private:
    QTreeWidget *m_treeWidget;
    QPoint m_dragStartPosition;
    QString m_lastOpenedDir;
    QString getConfigPath() const;
    QSet<QString> m_expandedPaths;

    void addAudioFilesToTree(QTreeWidgetItem *parent, const QString &dirPath);
    void clearAllDirectories();

    void searchDirectory(QTreeWidgetItem* parent, const QString& path,
                         const QString& filter, const QString& searchText,
                         bool &found, bool groupByMetadata);
    bool matchesSearch(const QFileInfo &fileInfo,
                       const QString &filter, const QString &searchText);
    AudioMetadataReader m_metadataReader;
    QCache<QString, AudioMetadata> m_metadataCache;
    void buildGroupedView(const QString& title, const QMap<QString, QList<AudioMetadata>>& groupedItems);
    void collectAllAudioFiles(QList<AudioMetadata>& results);
    void collectMatchingFiles(const QString &filter, const QString &searchText, QList<AudioMetadata>& results);
    void groupFiles(const QList<AudioMetadata>& files, const QString& groupBy, QMap<QString, QList<AudioMetadata>>& result);
};

#endif // TREEWIDGETWINDOW_H
