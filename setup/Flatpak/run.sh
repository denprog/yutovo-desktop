#!/bin/bash

file="$1"

cd /app/bin || exit 1

if [[ -n "$file" ]]; then
    LD_LIBRARY_PATH=/app/bin:$LD_LIBRARY_PATH ./yutovo-desktop "$file" --logs-path="$XDG_DATA_HOME/log"
else
    LD_LIBRARY_PATH=/app/bin:$LD_LIBRARY_PATH ./yutovo-desktop --logs-path="$XDG_DATA_HOME/log"
fi
