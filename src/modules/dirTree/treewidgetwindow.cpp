#include "treewidgetwindow.h"
#include <QFileDialog>
#include <QMenu>
#include <QDir>
#include <QStyle>
#include <qevent.h>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QStandardPaths>

TreeWidgetWindow::TreeWidgetWindow(QTreeWidget *treeWidget, QObject *parent)
    : QObject(parent), m_treeWidget(treeWidget), m_lastOpenedDir(), m_metadataCache(1000)
{
    m_treeWidget->setHeaderLabel("Audio Browser");
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &TreeWidgetWindow::onCustomContextMenuRequested);

    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Restore state immediately after construction
    restoreState();
}

void TreeWidgetWindow::clearAllDirectories()
{
    m_treeWidget->clear();
}

void TreeWidgetWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu(m_treeWidget->viewport());

    QTreeWidgetItem *clickedItem = m_treeWidget->itemAt(pos);
    QAction *addToQueueAction = contextMenu.addAction("Add to queue");
    QAction *addDirAction = contextMenu.addAction("Add Directory...");

    if (clickedItem) {
        QString filePath = clickedItem->data(0, Qt::UserRole).toString();
        bool isAudioFile = filePath.endsWith(".mp3") || filePath.endsWith(".wav") || filePath.endsWith(".flac");

        // Enable/disable actions based on item type
        addToQueueAction->setEnabled(isAudioFile);
        addDirAction->setEnabled(!isAudioFile);

        // Connect actions conditionally
        if (isAudioFile) {
            connect(addToQueueAction, &QAction::triggered, this, [this, filePath]() {
                emit fileAddRequested(filePath);
            });
        } else {
            connect(addDirAction, &QAction::triggered, this, [this, pos]() {
                addDirectoryToTree(pos);
            });
        }
    } else {
        // No item clicked - disable both actions
        addToQueueAction->setEnabled(false);
        addDirAction->setEnabled(true);
        connect(addDirAction, &QAction::triggered, this, [this, pos]() {
            addDirectoryToTree(pos);
        });
    }

    contextMenu.exec(m_treeWidget->viewport()->mapToGlobal(pos));
}

void TreeWidgetWindow::addDirectoryToTree(const QPoint &pos)
{
    QTreeWidgetItem *clickedItem = m_treeWidget->itemAt(pos);
    QString startDir = clickedItem ? clickedItem->data(0, Qt::UserRole).toString() : m_lastOpenedDir;

    QString dir = QFileDialog::getExistingDirectory(
        m_treeWidget,
        "Select Audio Directory",
        startDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dir.isEmpty()) {
        m_lastOpenedDir = dir; // Store the last opened directory
        clearAllDirectories();

        QTreeWidgetItem *newItem = new QTreeWidgetItem(m_treeWidget);
        newItem->setText(0, QDir(dir).dirName());
        newItem->setData(0, Qt::UserRole, dir);
        newItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_DirIcon));

        scanDirectory(newItem, dir);
        emit directoryAdded();
    }
}

void TreeWidgetWindow::scanDirectory(QTreeWidgetItem* parentItem, const QString& path)
{
    QDir dir(path);
    const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& dirInfo : dirs) {
        QTreeWidgetItem* dirItem = new QTreeWidgetItem(parentItem);
        dirItem->setText(0, dirInfo.fileName());
        dirItem->setData(0, Qt::UserRole, dirInfo.absoluteFilePath());
        dirItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_DirIcon));
        scanDirectory(dirItem, dirInfo.absoluteFilePath());
    }
    addAudioFilesToTree(parentItem, path);
}

void TreeWidgetWindow::addAudioFilesToTree(QTreeWidgetItem *parent, const QString &dirPath)
{
    QDir dir(dirPath);
    const QFileInfoList files = dir.entryInfoList(audioExtensions, QDir::Files);

    for (const QFileInfo &fileInfo : std::as_const(files)) {
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(parent);
        fileItem->setText(0, fileInfo.fileName());
        fileItem->setData(0, Qt::UserRole, fileInfo.absoluteFilePath());
        fileItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_FileIcon));
    }
}

QString TreeWidgetWindow::getConfigPath() const
{
    // First try application directory
    QString appDirPath = QCoreApplication::applicationDirPath();
    QFileInfo appDir(appDirPath);

    // Check if we can write to application directory
    if (appDir.isWritable()) {
        return appDirPath + "/config.ini";
    }

    // Fall back to AppData location if application directory isn't writable
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + "/" + QCoreApplication::applicationName() + "/config.ini";
}

void TreeWidgetWindow::saveState()
{
    QString configPath = getConfigPath();
    QFileInfo configFile(configPath);
    QDir().mkpath(configFile.absolutePath());

    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue("lastOpenedDir", m_lastOpenedDir);

    // Save expanded state
    QStringList expandedPaths;
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        if ((*it)->isExpanded() && !(*it)->data(0, Qt::UserRole).toString().isEmpty()) {
            expandedPaths << (*it)->data(0, Qt::UserRole).toString();
        }
        ++it;
    }
    settings.setValue("expandedPaths", expandedPaths);
}

void TreeWidgetWindow::restoreState()
{
    QString configPath = getConfigPath();
    if (!QFile::exists(configPath)) return;

    QSettings settings(configPath, QSettings::IniFormat);
    m_lastOpenedDir = settings.value("lastOpenedDir", "").toString();

    if (!m_lastOpenedDir.isEmpty() && QDir(m_lastOpenedDir).exists()) {
        clearAllDirectories();

        QTreeWidgetItem* newItem = new QTreeWidgetItem(m_treeWidget);
        newItem->setText(0, QDir(m_lastOpenedDir).dirName());
        newItem->setData(0, Qt::UserRole, m_lastOpenedDir);
        newItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_DirIcon));

        scanDirectory(newItem, m_lastOpenedDir);

        // Restore expanded state after directory is loaded
        QStringList expandedPaths = settings.value("expandedPaths").toStringList();
        QTreeWidgetItemIterator it(m_treeWidget);
        while (*it) {
            QString itemPath = (*it)->data(0, Qt::UserRole).toString();
            if (expandedPaths.contains(itemPath)) {
                (*it)->setExpanded(true);
            }
            ++it;
        }

        emit directoryAdded();
    }
}


// Search stuff
void TreeWidgetWindow::searchFiles(const QString &filter, const QString &searchText)
{
    if (m_lastOpenedDir.isEmpty()) return;

    clearAllDirectories();

    QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_treeWidget);
    rootItem->setText(0, "Search Results");
    rootItem->setData(0, Qt::UserRole, m_lastOpenedDir);
    rootItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_FileDialogContentsView));

    bool found = false;
    searchDirectory(rootItem, m_lastOpenedDir, filter, searchText, found);

    if (!found) {
        delete rootItem;
        QTreeWidgetItem* noResults = new QTreeWidgetItem(m_treeWidget);
        noResults->setText(0, "No results found");
        noResults->setFlags(noResults->flags() & ~Qt::ItemIsSelectable);
    } else {
        rootItem->setExpanded(true);
    }
}

void TreeWidgetWindow::searchDirectory(QTreeWidgetItem* parent, const QString& path,
                                       const QString& filter, const QString& searchText,
                                       bool &found)
{
    QDir dir(path);

    // Search in subdirectories first
    const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& dirInfo : dirs) {
        bool dirFound = false;
        QTreeWidgetItem* dirItem = new QTreeWidgetItem(parent);
        dirItem->setText(0, dirInfo.fileName());
        dirItem->setData(0, Qt::UserRole, dirInfo.absoluteFilePath());
        dirItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_DirIcon));

        searchDirectory(dirItem, dirInfo.absoluteFilePath(), filter, searchText, dirFound);

        if (dirFound) {
            found = true;
        } else {
            delete dirItem; // Remove empty directory items
        }
    }

    // Search in files
    const QFileInfoList files = dir.entryInfoList(audioExtensions, QDir::Files);
    for (const QFileInfo &fileInfo : files) {
        if (matchesSearch(fileInfo, filter, searchText)) {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(parent);
            fileItem->setText(0, fileInfo.fileName());
            fileItem->setData(0, Qt::UserRole, fileInfo.absoluteFilePath());
            fileItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_FileIcon));
            found = true;
        }
    }
}

bool TreeWidgetWindow::matchesSearch(const QFileInfo &fileInfo,
                                     const QString &filter, const QString &searchText)
{
    if (searchText.isEmpty()) return false;

    QString lowerSearch = searchText.toLower();
    QString filePath = fileInfo.absoluteFilePath();
    AudioMetadata* meta = m_metadataCache.object(filePath);

    if (!meta) {
        // Not in cache, read and store
        meta = new AudioMetadata(m_metadataReader.readMetadata(filePath));
        m_metadataCache.insert(filePath, meta);
    }

    if (filter == "File Names") {
        return fileInfo.fileName().toLower().contains(lowerSearch);
    }
    else if (filter == "Artist") {
        return meta->artist.toLower().contains(lowerSearch);
    }
    else if (filter == "Album") {
        return meta->album.toLower().contains(lowerSearch);
    }
    else if (filter == "Song Name") {
        return meta->title.toLower().contains(lowerSearch);
    }
    else if (filter == "Year") {
        // Note: You might want to add year to your AudioMetadata struct
        // For now, we'll skip year as it's not in your metadata struct
        return false;
    }
    else if (filter == "Genre") {
        return meta->genre.toLower().contains(lowerSearch);
    }

    return false;
}
