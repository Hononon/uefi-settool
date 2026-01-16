#ifndef LINUXBOOTMANAGER_H
#define LINUXBOOTMANAGER_H

#include "ibootmanager.h"

#ifdef Q_OS_LINUX

class LinuxBootManager : public IBootManager {
public:
    LinuxBootManager() = default;
    ~LinuxBootManager() override = default;

    bool load(QList<BootEntry>& entries, QList<uint16_t>& order) override;
    bool save(const QList<uint16_t>& order) override;

private:
    void parseEfibootmgrOutput(const QString& output, QList<BootEntry>& entries, QList<uint16_t>& order);
};

#endif // Q_OS_LINUX

#endif // LINUXBOOTMANAGER_H
