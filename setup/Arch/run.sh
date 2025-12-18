#!/bin/bash

file="$1"
LOGS="${HOME}/.local/share/yutovo/log"

mkdir -p "$LOGS"

if [[ -n "$file" ]]; then
    ./yutovo-desktop "$file" --logs-path="$LOGS"
else
    ./yutovo-desktop --logs-path="$LOGS"
fi
