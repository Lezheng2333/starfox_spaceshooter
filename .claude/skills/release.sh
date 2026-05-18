#!/bin/bash
# === STAR FOX SPACE SHOOTER — Release Script ===
# Usage: .claude/skills/release.sh <version> <title>
# Example: .claude/skills/release.sh 1.2.9 "Ver 1.2.9: description"
#
# Prerequisites (already done by Claude before running this):
#   1. DEVELOPMENT_LOG.md updated with new version entry
#   2. README.md updated (download links, What's New, project structure)
#   3. Source code version string updated
#   4. Code compiles cleanly
# This script handles: compile → package .app → sign → zip → git commit/push → gh release

set -e

VERSION="$1"
TITLE="$2"
NOTES="$3"

if [ -z "$VERSION" ] || [ -z "$TITLE" ]; then
    echo "Usage: release.sh <version> <title> [notes]"
    echo "Example: release.sh 1.2.9 'Ver 1.2.9: new features'"
    exit 1
fi

PROJECT_ROOT="/Users/song_jiarui/Code/Github/极简空间射击"
SRC_DIR="$PROJECT_ROOT/v2.0.0"
RELEASE_DIR="$SRC_DIR/Release Version"
APP_NAME="Shooter ver${VERSION}.app"
ZIP_NAME="Shooter ver${VERSION}.zip"
SRC_FILE="space_shooting ver2.0.0 developing.cpp"

echo "=== Step 1: Compile ==="
cd "$SRC_DIR"
clang++ -std=c++11 -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
    -D_THREAD_SAFE "$SRC_FILE" -o shooter \
    -L/opt/homebrew/lib -lSDL2
echo "Compile OK"

echo "=== Step 2: Package .app ==="
APP_DIR="$RELEASE_DIR/$APP_NAME"
rm -rf "$APP_DIR"
mkdir -p "$APP_DIR/Contents/MacOS"
mkdir -p "$APP_DIR/Contents/Frameworks"
mkdir -p "$APP_DIR/Contents/Resources"

cp shooter "$APP_DIR/Contents/MacOS/shooter"
cp /opt/homebrew/opt/sdl2/lib/libSDL2-2.0.0.dylib "$APP_DIR/Contents/Frameworks/"
install_name_tool -change /opt/homebrew/opt/sdl2/lib/libSDL2-2.0.0.dylib \
    @executable_path/../Frameworks/libSDL2-2.0.0.dylib \
    "$APP_DIR/Contents/MacOS/shooter"

cat > "$APP_DIR/Contents/Info.plist" << PLEOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>shooter</string>
    <key>CFBundleIdentifier</key>
    <string>com.starfox.spaceshooter</string>
    <key>CFBundleName</key>
    <string>Shooter</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    <key>CFBundleShortVersionString</key>
    <string>${VERSION}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
PLEOF

echo "=== Step 3: Code Sign ==="
codesign --force --deep --sign - "$APP_DIR"
echo "Sign OK"

echo "=== Step 4: Create .zip ==="
cd "$RELEASE_DIR"
ditto -c -k --keepParent "$APP_NAME" "$ZIP_NAME"
ls -la "$ZIP_NAME"
echo "Zip OK"

echo "=== Step 5: Git commit & push ==="
cd "$PROJECT_ROOT"
git add DEVELOPMENT_LOG.md README.md "$SRC_DIR/$SRC_FILE" "$RELEASE_DIR/$ZIP_NAME"
git commit -m "$TITLE"
git push origin main
echo "Push OK"

echo "=== Step 6: GitHub Release ==="
if [ -n "$NOTES" ]; then
    gh release create "v${VERSION}" --title "$TITLE" --notes "$NOTES" "$RELEASE_DIR/$ZIP_NAME"
else
    gh release create "v${VERSION}" --title "$TITLE" "$RELEASE_DIR/$ZIP_NAME"
fi
echo "=== Release v${VERSION} complete! ==="
echo "URL: https://github.com/Lezheng2333/starfox_spaceshooter/releases/tag/v${VERSION}"
