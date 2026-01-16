#include "winbootmanager.h"

#ifdef Q_OS_WIN

WinBootManager::WinBootManager() {
    enablePrivilege();
}

bool WinBootManager::enablePrivilege() {
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!LookupPrivilegeValueW(nullptr, SE_SYSTEM_ENVIRONMENT_NAME, &tp.Privileges[0].Luid)) {
        CloseHandle(hToken);
        return false;
    }

    bool result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr);
    DWORD err = GetLastError();
    CloseHandle(hToken);

    return result && (err == ERROR_SUCCESS);
}

bool WinBootManager::load(QList<BootEntry>& entries, QList<uint16_t>& order) {
    entries.clear();
    order.clear();

    // Read BootOrder variable
    BYTE bootOrderBuffer[4096];
    DWORD bootOrderSize = GetFirmwareEnvironmentVariableW(
        L"BootOrder",
        EFI_GLOBAL_GUID,
        bootOrderBuffer,
        sizeof(bootOrderBuffer)
    );

    if (bootOrderSize == 0) {
        return false;
    }

    // Parse BootOrder (array of uint16_t)
    int orderCount = bootOrderSize / sizeof(uint16_t);
    uint16_t* orderArray = reinterpret_cast<uint16_t*>(bootOrderBuffer);
    for (int i = 0; i < orderCount; ++i) {
        order.append(orderArray[i]);
    }

    // Read each Boot#### variable
    for (uint16_t bootId : order) {
        wchar_t varName[16];
        swprintf_s(varName, L"Boot%04X", bootId);

        BYTE optionBuffer[4096];
        DWORD optionSize = GetFirmwareEnvironmentVariableW(
            varName,
            EFI_GLOBAL_GUID,
            optionBuffer,
            sizeof(optionBuffer)
        );

        if (optionSize > 0) {
            QByteArray data(reinterpret_cast<char*>(optionBuffer), optionSize);
            
            BootEntry entry;
            entry.id = bootId;
            entry.name = parseBootOptionName(data);
            // Check LOAD_OPTION_ACTIVE flag (bit 0 of Attributes)
            entry.active = (optionSize >= 4) && (optionBuffer[0] & 0x01);
            
            entries.append(entry);
        }
    }

    return true;
}

QString WinBootManager::parseBootOptionName(const QByteArray& data) {
    // EFI_LOAD_OPTION structure:
    // Offset 0: Attributes (4 bytes)
    // Offset 4: FilePathListLength (2 bytes)
    // Offset 6: Description (null-terminated UTF-16 string)
    
    if (data.size() < 6) {
        return QString();
    }

    const char* ptr = data.constData() + 6;
    int remaining = data.size() - 6;

    QString name;
    const ushort* utf16Ptr = reinterpret_cast<const ushort*>(ptr);
    int maxChars = remaining / sizeof(ushort);

    for (int i = 0; i < maxChars; ++i) {
        if (utf16Ptr[i] == 0) {
            break;
        }
        name.append(QChar(utf16Ptr[i]));
    }

    return name;
}

bool WinBootManager::save(const QList<uint16_t>& order) {
    if (order.isEmpty()) {
        return false;
    }

    QByteArray bootOrderData;
    bootOrderData.reserve(order.size() * sizeof(uint16_t));

    for (uint16_t id : order) {
        bootOrderData.append(reinterpret_cast<const char*>(&id), sizeof(uint16_t));
    }

    BOOL result = SetFirmwareEnvironmentVariableW(
        L"BootOrder",
        EFI_GLOBAL_GUID,
        bootOrderData.data(),
        bootOrderData.size()
    );

    return result != FALSE;
}

#endif // Q_OS_WIN
