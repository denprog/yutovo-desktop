#!/usr/bin/env bash

set -euo pipefail

find ./library -type f -name "*.yut" -print0 | while IFS= read -r -d '' file; do
    if gzip -t "$file" 2>/dev/null; then
        echo "Unpacking: $file"

        tmp="${file}.tmp_unpacked"

        gzip -dc "$file" > "$tmp"
        mv "$tmp" "$file"
    else
        echo "Skipping (not gzip): $file"
    fi
done

echo "Done."