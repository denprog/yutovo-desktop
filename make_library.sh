#!/usr/bin/env bash
# gen_from_in.sh — recursively generates files from *.in using cpp (similar to #ifdef)
#
# Usage:
#   ./gen_from_in.sh <output_directory> [WEB] [ZIP]
#
# Examples:
#   ./gen_from_in.sh build/web WEB
#   ./gen_from_in.sh build/web ZIP
#   ./gen_from_in.sh build/web WEB ZIP
#   ./gen_from_in.sh build/web ZIP WEB

set -euo pipefail

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <output_directory> [WEB] [ZIP]" >&2
    exit 1
fi

OUT_DIR="$1"
shift

MACRO=""
ZIP=false

for arg in "$@"; do
    case "$arg" in
        ZIP)
            ZIP=true
            ;;
        *)
            if [[ -n "$MACRO" ]]; then
                echo "Error: multiple macros specified: '$MACRO' and '$arg'" >&2
                exit 1
            fi
            MACRO="$arg"
            ;;
    esac
done

SRC_DIR="$(pwd)"

mkdir -p "$OUT_DIR"
OUT_DIR="$(realpath "$OUT_DIR")"

CPP_FLAGS=(-P -x c)
if [[ -n "$MACRO" ]]; then
    CPP_FLAGS+=("-D$MACRO")
fi

# Exclude the output directory from the scan if it is inside the source directory
PRUNE_ARGS=()
if [[ "$OUT_DIR" == "$SRC_DIR"/* ]]; then
    rel_out="${OUT_DIR#"$SRC_DIR"/}"
    top="${rel_out%%/*}"
    PRUNE_ARGS=(-path "./$top" -prune -o)
fi

find ./library "${PRUNE_ARGS[@]}" -type f -print | while read -r f; do
    rel="${f#./}"
    dst="$OUT_DIR/$rel"
    mkdir -p "$(dirname "$dst")"

    if [[ "$rel" == *.in ]]; then
        dst="${dst%.in}"
        echo "GEN : $rel -> ${dst#"$OUT_DIR"/}"
        cpp "${CPP_FLAGS[@]}" "$f" "$dst"
    else
        echo "COPY: $rel"
        cp -p "$f" "$dst"
    fi

    if $ZIP && [[ "$dst" == *.yut ]]; then
        echo "ZIP : ${dst#"$OUT_DIR"/}"
        tmp="${dst}.tmp_gzip"
        gzip -c "$dst" > "$tmp"
        mv "$tmp" "$dst"
    fi
done

echo "Done: $OUT_DIR"
