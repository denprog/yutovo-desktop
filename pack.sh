#!/usr/bin/env bash

set -euo pipefail

find ./library -type f -name "*.yut" -print0 | while IFS= read -r -d '' file; do
    echo "Packing: $file"

    tmp="${file}.tmp_gzip"

    # pack
    gzip -c "$file" > "$tmp"

    # replace source file
    mv "$tmp" "$file"
done

echo "Done."