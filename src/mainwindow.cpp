#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QIcon>

#ifdef Q_OS_WIN
#include "winbootmanager.h"
#elif defined(Q_OS_LINUX)
#include "linuxbootmanager.h"
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
#ifdef Q_OS_WIN
    m_bootManager = std::make_unique<WinBootManager>();
#elif defined(Q_OS_LINUX)
    m_bootManager = std::make_unique<LinuxBootManager>();
#endif

    setupUi();
    onRefresh();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle(tr("UEFI Boot Order Manager"));
    setMinimumSize(450, 400);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // List widget
    m_listWidget = new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
    mainLayout->addWidget(m_listWidget);

    // Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_btnRefresh = new QPushButton(tr("Refresh"), this);
    m_btnMoveUp = new QPushButton(tr("Move Up"), this);
    m_btnMoveDown = new QPushButton(tr("Move Down"), this);
    m_btnApply = new QPushButton(tr("Apply"), this);

    buttonLayout->addWidget(m_btnRefresh);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_btnMoveUp);
    buttonLayout->addWidget(m_btnMoveDown);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_btnApply);

    mainLayout->addLayout(buttonLayout);

    // Status label
    m_statusLabel = new QLabel(this);
    mainLayout->addWidget(m_statusLabel);

    // Connections
    connect(m_btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefresh);
    connect(m_btnMoveUp, &QPushButton::clicked, this, &MainWindow::onMoveUp);
    connect(m_btnMoveDown, &QPushButton::clicked, this, &MainWindow::onMoveDown);
    connect(m_btnApply, &QPushButton::clicked, this, &MainWindow::onApply);
}

void MainWindow::onRefresh() {
    if (!m_bootManager) {
        m_statusLabel->setText(tr("Error: Boot manager not available on this platform"));
        return;
    }

    m_entries.clear();
    m_order.clear();

    if (m_bootManager->load(m_entries, m_order)) {
        updateList();
        m_statusLabel->setText(tr("Loaded %1 boot entries").arg(m_entries.size()));
    } else {
        m_statusLabel->setText(tr("Failed to load boot entries. Run as administrator/root."));
    }
}

void MainWindow::updateList() {
    m_listWidget->clear();

    // Create a map for quick lookup
    QMap<uint16_t, BootEntry> entryMap;
    for (const BootEntry &entry : m_entries) {
        entryMap[entry.id] = entry;
    }

    // Display entries in boot order
    for (uint16_t id : m_order) {
        if (entryMap.contains(id)) {
            const BootEntry &entry = entryMap[id];
            QString text = QString("Boot%1: %2%3")
                .arg(id, 4, 16, QChar('0'))
                .arg(entry.name)
                .arg(entry.active ? "" : " [Inactive]");
            
            QListWidgetItem *item = new QListWidgetItem(text);
            item->setData(Qt::UserRole, id);
            if (!entry.active) {
                item->setForeground(Qt::gray);
            }
            m_listWidget->addItem(item);
        }
    }
}

void MainWindow::onMoveUp() {
    int row = m_listWidget->currentRow();
    if (row <= 0) return;

    swapItems(row, row - 1);
    m_listWidget->setCurrentRow(row - 1);
}

void MainWindow::onMoveDown() {
    int row = m_listWidget->currentRow();
    if (row < 0 || row >= m_listWidget->count() - 1) return;

    swapItems(row, row + 1);
    m_listWidget->setCurrentRow(row + 1);
}

void MainWindow::swapItems(int from, int to) {
    // Swap in order list
    if (from >= 0 && from < m_order.size() && to >= 0 && to < m_order.size()) {
        std::swap(m_order[from], m_order[to]);
    }

    // Swap in list widget
    QListWidgetItem *item = m_listWidget->takeItem(from);
    m_listWidget->insertItem(to, item);
}

void MainWindow::onApply() {
    if (!m_bootManager) {
        QMessageBox::critical(this, tr("Error"), tr("Boot manager not available"));
        return;
    }

    if (m_bootManager->save(m_order)) {
        m_statusLabel->setText(tr("Boot order saved successfully"));
        QMessageBox::information(this, tr("Success"), tr("Boot order has been updated."));
    } else {
        m_statusLabel->setText(tr("Failed to save boot order"));
        QMessageBox::critical(this, tr("Error"), 
            tr("Failed to save boot order. Make sure you have administrator/root privileges."));
    }
}
