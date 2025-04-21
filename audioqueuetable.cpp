#include "audioqueuetable.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QFileInfo>
#include <QHeaderView>
#include <QMimeData>

AudioQueueManager::AudioQueueManager(QTableWidget *tableWidget, QObject *parent)
    : QObject(parent), m_tableWidget(tableWidget)
{
    setupTable();
    m_contextMenu = new QMenu(m_tableWidget);
    QAction *removeAction = m_contextMenu->addAction("Remove from queue");
    connect(removeAction, &QAction::triggered, this, &AudioQueueManager::removeSelectedRows);
}

void AudioQueueManager::setupTable()
{
    // Set 3 columns with headers
    m_tableWidget->setColumnCount(3);
    m_tableWidget->setHorizontalHeaderLabels({"Name", "Artist", "Duration"});

    // Configure sizing
    m_tableWidget->verticalHeader()->setDefaultSectionSize(24); // Fixed row height
    m_tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Set column widths
    m_tableWidget->setColumnWidth(0, 200); // Name
    m_tableWidget->setColumnWidth(1, 150); // Artist
    m_tableWidget->setColumnWidth(2, 80);  // Duration

    // Other existing setup
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setAcceptDrops(true);
    m_tableWidget->setDragDropMode(QAbstractItemView::DropOnly);
    m_tableWidget->viewport()->installEventFilter(this);
    m_tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void AudioQueueManager::setupConnections()
{
    connect(m_tableWidget, &QTableWidget::customContextMenuRequested,
            [this](const QPoint &pos) {
                if (m_tableWidget->selectedItems().count() > 0) {
                    m_contextMenu->exec(m_tableWidget->viewport()->mapToGlobal(pos));
                }
            });
}

void AudioQueueManager::addToQueue(const QString &filePath)
{
    // Validate file exists first
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        qWarning() << "File does not exist:" << filePath;
        return;
    }

    // Read metadata (with fallback to filename)
    AudioMetadata meta = m_metadataReader.readMetadata(filePath);

    // Insert new row
    const int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);

    // Column 0: Track Name
    QTableWidgetItem *nameItem = new QTableWidgetItem(
        meta.title.isEmpty() ? fileInfo.completeBaseName() : meta.title
        );
    m_tableWidget->setItem(row, 0, nameItem);

    // Column 1: Artist (with fallback handling)
    QTableWidgetItem *artistItem = new QTableWidgetItem(meta.artist);
    m_tableWidget->setItem(row, 1, artistItem);

    // Column 2: Duration (formatted)
    QTableWidgetItem *durationItem = new QTableWidgetItem(meta.duration);
    //durationItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_tableWidget->setItem(row, 2, durationItem);

    // Store complete metadata in the first column's UserRole
    nameItem->setData(Qt::UserRole, QVariant::fromValue(meta));

    // Additional metadata storage (optional)
    artistItem->setData(Qt::UserRole, meta.artist);
    durationItem->setData(Qt::UserRole, meta.durationMs);

    m_tableWidget->setRowHeight(row, 24);
}

void AudioQueueManager::removeSelectedRows()
{
    QList<QTableWidgetItem*> items = m_tableWidget->selectedItems();
    QSet<int> rows;

    foreach(QTableWidgetItem *item, items) {
        rows.insert(item->row());
    }

    QList<int> sortedRows = rows.values();
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

    foreach(int row, sortedRows) {
        m_tableWidget->removeRow(row);
    }
}

bool AudioQueueManager::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_tableWidget->viewport()) {
        switch (event->type()) {
        case QEvent::DragEnter:
            static_cast<QDragEnterEvent*>(event)->acceptProposedAction();
            return true;
        case QEvent::Drop:
            handleDropEvent(static_cast<QDropEvent*>(event));
            return true;
        default:
            break;
        }
    }
    return QObject::eventFilter(watched, event);
}

