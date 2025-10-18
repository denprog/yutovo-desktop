#!/bin/bash

file="$1"

cd $SNAP/app/bin || exit 1

if [[ -n "$file" ]]; then
    ./yutovo-desktop "$file" --logs-path="$SNAP_USER_COMMON/log"
else
    ./yutovo-desktop --logs-path="$SNAP_USER_COMMON/log"
fi
