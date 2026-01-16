#include <QApplication>
#include "mainwindow.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>

bool isElevated() {
    BOOL elevated = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD size = sizeof(elevation);
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size)) {
            elevated = elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return elevated;
}

bool requestElevation() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = path;
    sei.hwnd = NULL;
    sei.nShow = SW_NORMAL;
    
    return ShellExecuteExW(&sei);
}
#endif

int main(int argc, char *argv[]) {
#ifdef _WIN32
    if (!isElevated()) {
        if (requestElevation()) {
            return 0;  // Exit current process, elevated one will start
        }
        // User denied elevation, continue anyway (will fail when accessing EFI vars)
    }
#endif

    QApplication app(argc, argv);
    
    app.setApplicationName("UEFI Boot Order Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("uefi-set-tool");

    MainWindow window;
    window.show();

    return app.exec();
}
