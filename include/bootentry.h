#ifndef BOOTENTRY_H
#define BOOTENTRY_H

#include <QString>
#include <cstdint>

struct BootEntry {
    uint16_t id;
    QString name;
    bool active;
};

#endif // BOOTENTRY_H
