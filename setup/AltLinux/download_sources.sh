#!/bin/bash
set -euo pipefail

SPEC="${1:-yutovo-desktop.spec}"
DEST="${HOME}/RPM/SOURCES"

mkdir -p "$DEST"

rpmspec -P "$SPEC" |
grep '^Source[0-9]*:' |
while read -r _ url; do
    if [[ "$url" == *"#/"* ]]; then
        download_url="${url%%#/*}"
        filename="${url##*/#}"
        filename="${url##*#/}"
    else
        download_url="$url"
        filename="$(basename "$url")"
    fi

    echo "Downloading $filename..."
    wget -c -O "$DEST/$filename" "$download_url"
done