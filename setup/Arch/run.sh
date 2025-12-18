#!/bin/bash

file="$1"

cd /opt/yutovo || exit 1

if [[ -n "$file" ]]; then
    ./yutovo-desktop "$file" --logs-path="/opt/yutovo/log"
else
    ./yutovo-desktop --logs-path="/opt/yutovo/log"
fi
