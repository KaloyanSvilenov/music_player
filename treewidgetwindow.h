#ifndef TREEWIDGETWINDOW_H
#define TREEWIDGETWINDOW_H

#include <QObject>
#include <QTreeWidget>
#include <QDir>
#include <QSettings>

class TreeWidgetWindow : public QObject
{
    Q_OBJECT
public:
    explicit TreeWidgetWindow(QTreeWidget *treeWidget, QObject *parent = nullptr);

    const QStringList audioExtensions = {"*.mp3", "*.wav", "*.flac"};

    void saveState();
    void restoreState();

signals:
    void fileAddRequested(const QString &filePath);
    void directoryAdded();

public slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void addDirectoryToTree(const QPoint &pos);
    void scanDirectory(QTreeWidgetItem *parentItem, const QString &path);

private:
    QTreeWidget *m_treeWidget;
    QPoint m_dragStartPosition;
    QString m_lastOpenedDir;

    void addAudioFilesToTree(QTreeWidgetItem *parent, const QString &dirPath);
    void clearAllDirectories();
    QString getConfigPath() const;
};

#endif // TREEWIDGETWINDOW_H
