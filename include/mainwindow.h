#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include "bootentry.h"
#include "ibootmanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onRefresh();
    void onMoveUp();
    void onMoveDown();
    void onApply();

private:
    void setupUi();
    void updateList();
    void swapItems(int from, int to);

    QListWidget *m_listWidget;
    QPushButton *m_btnRefresh;
    QPushButton *m_btnMoveUp;
    QPushButton *m_btnMoveDown;
    QPushButton *m_btnApply;
    QLabel *m_statusLabel;

    std::unique_ptr<IBootManager> m_bootManager;
    QList<BootEntry> m_entries;
    QList<uint16_t> m_order;
};

#endif // MAINWINDOW_H
