#include "treewidgetwindow.h"
#include <QFileDialog>
#include <QMenu>
#include <QDir>
#include <QStyle>
#include <qevent.h>
#include <QDrag>
#include <QMimeData>

TreeWidgetWindow::TreeWidgetWindow(QTreeWidget *treeWidget, QObject *parent)
    : QObject(parent), m_treeWidget(treeWidget)
{
    m_treeWidget->setHeaderLabel("Audio Browser");
    m_treeWidget->setColumnCount(1);
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &TreeWidgetWindow::onCustomContextMenuRequested);

    m_treeWidget->setDragEnabled(true);
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeWidget->setDragDropMode(QAbstractItemView::DragOnly);
}

void TreeWidgetWindow::clearAllDirectories()
{
    m_treeWidget->clear();  // Remove all items from the tree
}

void TreeWidgetWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu;
    QAction *addDirAction = contextMenu.addAction("Add Directory...");
    connect(addDirAction, &QAction::triggered, [this, pos]() {
        addDirectoryToTree(pos);
    });
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
        // Clear all existing directories before adding new one
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
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &dirInfo : dirs) {
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
    QFileInfoList files = dir.entryInfoList(audioExtensions, QDir::Files);
    for (const QFileInfo &fileInfo : files) {
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(parent);
        fileItem->setText(0, fileInfo.fileName());
        fileItem->setData(0, Qt::UserRole, fileInfo.absoluteFilePath());
        fileItem->setIcon(0, m_treeWidget->style()->standardIcon(QStyle::SP_FileIcon));
    }
}

void TreeWidgetWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QTreeWidgetItem *item = currentItem();
        if (item) {
            QDrag *drag = new QDrag(this);
            QMimeData *mime = new QMimeData;
            mime->setText(item->data(0, Qt::UserRole).toString());
            drag->setMimeData(mime);
            drag->exec(Qt::CopyAction);
        }
    }
    QTreeWidget::mouseMoveEvent(event);
}
