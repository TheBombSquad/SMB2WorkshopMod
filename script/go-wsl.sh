#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

ROMHACK_DIR=/mnt/c/Users/ComplexPlane/Documents/projects/romhack
PPCINJECT=~/ppc-inject/PPCInject
DOL_SRC=$ROMHACK_DIR/smb2imm/sys/main.dol
DOL_DEST=$ROMHACK_DIR/smb2mut/sys/main.dol
REL_SRC=$ROMHACK_DIR/smb2imm/files/mkb2.rel_sample.rel
REL_DEST=$ROMHACK_DIR/smb2mut/files/mkb2.rel_sample.rel
RELLOADER_SRC=relloader/iso-rel-loader-us.asm
CARD_DIR=/mnt/c/Users/ComplexPlane/Documents/Dolphin\ Emulator/GC/USA/Card\ A

if [[ "${1:-}" == "-u" ]]; then
  # Unpatch
  echo 'Resetting main.dol to vanilla'
  cp "$DOL_SRC" "$DOL_DEST"
  cp "$REL_SRC" "$REL_DEST"
  exit 0
fi

if [[ "${1:-}" == "-i" ]]; then
    echo 'Patching main.dol with ppcinject'
    "$PPCINJECT" "$DOL_SRC" "$DOL_DEST" "$RELLOADER_SRC"
fi

make -j$(nproc)

echo 'Copying REL to smb2mut'
cp mkb2.rel_sample.rel "$REL_DEST"
