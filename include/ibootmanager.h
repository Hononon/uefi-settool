#ifndef IBOOTMANAGER_H
#define IBOOTMANAGER_H

#include "bootentry.h"
#include <QList>

class IBootManager {
public:
    virtual ~IBootManager() = default;
    virtual bool load(QList<BootEntry>& entries, QList<uint16_t>& order) = 0;
    virtual bool save(const QList<uint16_t>& order) = 0;
};

#endif // IBOOTMANAGER_H
