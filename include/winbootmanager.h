#ifndef WINBOOTMANAGER_H
#define WINBOOTMANAGER_H

#include "ibootmanager.h"

#ifdef Q_OS_WIN

#include <windows.h>

class WinBootManager : public IBootManager {
public:
    WinBootManager();
    ~WinBootManager() override = default;

    bool load(QList<BootEntry>& entries, QList<uint16_t>& order) override;
    bool save(const QList<uint16_t>& order) override;

private:
    bool enablePrivilege();
    QString parseBootOptionName(const QByteArray& data);

    static constexpr const wchar_t* EFI_GLOBAL_GUID = L"{8BE4DF61-93CA-11D2-AA0D-00E098032B8C}";
};

#endif // Q_OS_WIN

#endif // WINBOOTMANAGER_H
