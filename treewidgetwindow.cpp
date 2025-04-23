#include "treewidgetwindow.h"
#include <QFileDialog>
#include <QMenu>
#include <QDir>
#include <QStyle>
#include <qevent.h>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

TreeWidgetWindow::TreeWidgetWindow(QTreeWidget *treeWidget, QObject *parent)
    : QObject(parent), m_treeWidget(treeWidget)
{
    m_treeWidget->setHeaderLabel("Audio Browser");
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &TreeWidgetWindow::onCustomContextMenuRequested);

    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
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
            connect(addToQueueAction, &QAction::triggered, [this, filePath]() {
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
    QString startDir = clickedItem ? clickedItem->data(0, Qt::UserRole).toString() : QDir::homePath();

    QString dir = QFileDialog::getExistingDirectory(
        m_treeWidget,
        "Select Audio Directory",
        startDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dir.isEmpty()) {
        clearAllDirectories();

        QTreeWidgetItem *newItem = new QTreeWidgetItem(m_treeWidget);
        newItem->setText(0, QDir(dir).dirName());
        newItem->setData(0, Qt::UserRole, dir);
        newItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_DirIcon));

        scanDirectory(newItem, dir);
    }
}

void TreeWidgetWindow::scanDirectory(QTreeWidgetItem *parentItem, const QString &path)
{
    QDir dir(path);
    const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &dirInfo : std::as_const(dirs)) {
        QTreeWidgetItem *dirItem = new QTreeWidgetItem(parentItem);
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
