#!/bin/bash

file="$1"

cd /app/bin || exit 1

if [[ -n "$file" ]]; then
    ./yutovo_desktop "$file" --logs-path="$XDG_DATA_HOME/log"
else
    ./yutovo_desktop --logs-path="$XDG_DATA_HOME/log"
fi
