#!/bin/bash
# Build script for Linux native build with optional .deb packaging

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build-linux"
PKG_NAME="uefiSetTool"
PKG_VERSION="1.0.0"
PKG_ARCH="amd64"

echo "=== Building UEFI Boot Order Manager (Linux) ==="

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "=== Build complete ==="
echo "Binary: $BUILD_DIR/uefi-settool"

# Build .deb package if requested
if [ "$1" = "--deb" ] || [ "$1" = "-d" ]; then
    echo ""
    echo "=== Building .deb package ==="
    
    DEB_DIR="$BUILD_DIR/deb-pkg"
    rm -rf "$DEB_DIR"
    mkdir -p "$DEB_DIR/DEBIAN"
    mkdir -p "$DEB_DIR/usr/bin"
    mkdir -p "$DEB_DIR/usr/share/applications"
    mkdir -p "$DEB_DIR/usr/share/doc/$PKG_NAME"
    mkdir -p "$DEB_DIR/usr/share/polkit-1/actions"

    # Copy binary
    cp "$BUILD_DIR/uefi-settool" "$DEB_DIR/usr/bin/"
    chmod 755 "$DEB_DIR/usr/bin/uefi-settool"

    # Create control file
    cat > "$DEB_DIR/DEBIAN/control" << EOF
Package: $PKG_NAME
Version: $PKG_VERSION
Section: admin
Priority: optional
Architecture: $PKG_ARCH
Depends: libqt6widgets6 (>= 6.2), libqt6gui6 (>= 6.2), libqt6core6 (>= 6.2), efibootmgr, policykit-1
Maintainer: Developer <dev@example.com>
Description: UEFI Boot Order Manager
 A Qt6-based graphical tool for managing UEFI boot order.
 Allows viewing, reordering, and saving UEFI boot entries.
EOF

    # Create desktop entry
    cat > "$DEB_DIR/usr/share/applications/$PKG_NAME.desktop" << EOF
[Desktop Entry]
Name=UEFI Boot Manager
Comment=Manage UEFI boot order
Exec=pkexec --disable-internal-agent /usr/bin/uefi-settool
Icon=preferences-system
Terminal=false
Type=Application
Categories=System;Settings;
EOF

    # Create polkit policy
    cat > "$DEB_DIR/usr/share/polkit-1/actions/com.github.uefi-settool.policy" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE policyconfig PUBLIC
 "-//freedesktop//DTD PolicyKit Policy Configuration 1.0//EN"
 "http://www.freedesktop.org/standards/PolicyKit/1/policyconfig.dtd">
<policyconfig>
  <action id="com.github.uefi-settool.run">
    <description>Run UEFI Boot Manager</description>
    <message>Authentication is required to manage UEFI boot order</message>
    <icon_name>preferences-system</icon_name>
    <defaults>
      <allow_any>auth_admin</allow_any>
      <allow_inactive>auth_admin</allow_inactive>
      <allow_active>auth_admin</allow_active>
    </defaults>
    <annotate key="org.freedesktop.policykit.exec.path">/usr/bin/uefi-settool</annotate>
    <annotate key="org.freedesktop.policykit.exec.allow_gui">true</annotate>
  </action>
</policyconfig>
EOF

    # Create copyright file
    cat > "$DEB_DIR/usr/share/doc/$PKG_NAME/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: $PKG_NAME
License: MIT
EOF

    # Build the package
    dpkg-deb --build "$DEB_DIR" "$BUILD_DIR/${PKG_NAME}_${PKG_VERSION}_${PKG_ARCH}.deb"

    echo "=== .deb package created ==="
    echo "Package: $BUILD_DIR/${PKG_NAME}_${PKG_VERSION}_${PKG_ARCH}.deb"
fi
