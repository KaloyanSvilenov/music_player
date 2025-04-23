#ifndef TREEWIDGETWINDOW_H
#define TREEWIDGETWINDOW_H

#include <QObject>
#include <QTreeWidget>
#include <QDir>

class TreeWidgetWindow : public QObject
{
    Q_OBJECT
public:
    explicit TreeWidgetWindow(QTreeWidget *treeWidget, QObject *parent = nullptr);

    const QStringList audioExtensions = {"*.mp3", "*.wav", "*.flac"};

signals:
    void fileAddRequested(const QString &filePath);

public slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void addDirectoryToTree(const QPoint &pos);
    void scanDirectory(QTreeWidgetItem *parentItem, const QString &path);

private:
    QTreeWidget *m_treeWidget;
    QPoint m_dragStartPosition;

    void addAudioFilesToTree(QTreeWidgetItem *parent, const QString &dirPath);
    void clearAllDirectories();
};

#endif // TREEWIDGETWINDOW_H
