#!/usr/bin/env bash

FIFO_PATH=$XDG_RUNTIME_DIR/layout_fifo_pipe

if [[ ! -p "$FIFO_PATH" ]]; then
    mkfifo "$FIFO_PATH" 2>&1
fi

../out/xkb-get-layout > "$FIFO_PATH"
