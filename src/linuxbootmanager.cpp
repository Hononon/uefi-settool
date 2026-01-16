#include "linuxbootmanager.h"

#ifdef Q_OS_LINUX

#include <QProcess>
#include <QRegularExpression>

bool LinuxBootManager::load(QList<BootEntry>& entries, QList<uint16_t>& order) {
    entries.clear();
    order.clear();

    QProcess process;
    process.start("efibootmgr", QStringList());
    
    if (!process.waitForFinished(5000)) {
        return false;
    }

    if (process.exitCode() != 0) {
        return false;
    }

    QString output = QString::fromUtf8(process.readAllStandardOutput());
    parseEfibootmgrOutput(output, entries, order);

    return !entries.isEmpty();
}

void LinuxBootManager::parseEfibootmgrOutput(const QString& output, QList<BootEntry>& entries, QList<uint16_t>& order) {
    // Example output:
    // BootCurrent: 0001
    // Timeout: 1 seconds
    // BootOrder: 0001,0002,0000
    // Boot0000* Windows Boot Manager
    // Boot0001* ubuntu
    // Boot0002  Network Boot

    QStringList lines = output.split('\n');

    // Parse BootOrder line
    static QRegularExpression bootOrderRe(R"(^BootOrder:\s*(.+)$)");
    // Parse Boot#### entries: * means active
    static QRegularExpression bootEntryRe(R"(^Boot([0-9A-Fa-f]{4})(\*?)\s+(.+)$)");

    for (const QString& line : lines) {
        QRegularExpressionMatch orderMatch = bootOrderRe.match(line);
        if (orderMatch.hasMatch()) {
            QString orderStr = orderMatch.captured(1);
            QStringList orderParts = orderStr.split(',', Qt::SkipEmptyParts);
            for (const QString& part : orderParts) {
                bool ok;
                uint16_t id = part.trimmed().toUInt(&ok, 16);
                if (ok) {
                    order.append(id);
                }
            }
            continue;
        }

        QRegularExpressionMatch entryMatch = bootEntryRe.match(line);
        if (entryMatch.hasMatch()) {
            bool ok;
            uint16_t id = entryMatch.captured(1).toUInt(&ok, 16);
            if (ok) {
                BootEntry entry;
                entry.id = id;
                entry.active = !entryMatch.captured(2).isEmpty(); // * means active
                entry.name = entryMatch.captured(3).trimmed();
                entries.append(entry);
            }
        }
    }
}

bool LinuxBootManager::save(const QList<uint16_t>& order) {
    if (order.isEmpty()) {
        return false;
    }

    // Build order string: 0001,0002,0000
    QStringList orderStrList;
    for (uint16_t id : order) {
        orderStrList.append(QString("%1").arg(id, 4, 16, QChar('0')).toUpper());
    }
    QString orderStr = orderStrList.join(',');

    QProcess process;
    process.start("pkexec", QStringList() << "efibootmgr" << "-o" << orderStr);

    if (!process.waitForFinished(30000)) {
        return false;
    }

    return process.exitCode() == 0;
}

#endif // Q_OS_LINUX
