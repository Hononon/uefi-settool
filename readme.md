# UEFI Boot Order Manager

A cross-platform Qt6 application for managing UEFI boot order.

## Features

- View all UEFI boot entries
- Reorder boot entries (move up/down)
- Save boot order changes
- Cross-platform support (Windows and Linux)

## Requirements

- Qt 6.2 or higher
- CMake 3.16 or higher
- C++17 compatible compiler

### Platform-specific requirements

**Windows:**
- Run as Administrator to modify boot settings

**Linux:**
- `efibootmgr` package installed
- `polkit` for privilege elevation (pkexec)

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

1. Run the application (with administrator/root privileges)
2. View current boot entries and their order
3. Select an entry and use "Move Up" / "Move Down" to reorder
4. Click "Apply" to save changes

## Project Structure

```
uefi-settool/
├── CMakeLists.txt
├── main.cpp
├── include/
│   ├── bootentry.h        # BootEntry struct
│   ├── ibootmanager.h     # Abstract interface
│   ├── winbootmanager.h   # Windows implementation
│   ├── linuxbootmanager.h # Linux implementation
│   └── mainwindow.h       # Main UI window
└── src/
    ├── winbootmanager.cpp
    ├── linuxbootmanager.cpp
    └── mainwindow.cpp
```

## License

MIT License
