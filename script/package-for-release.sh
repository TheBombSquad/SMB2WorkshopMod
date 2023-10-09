#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

OUT_NAME=ApeSphere-Custom-v0.4.0.zip

mkdir -p out
make -j$(nproc)
# TODO: We need to make PPCInject and add the executable here!
cp LICENSE README.md mkb2.rel_sample.rel docs/music-id-list.txt docs/theme-id-list.txt relloader/iso-rel-loader-us.asm out
cp configs/default-config.txt out/config.txt
cp configs/default-authors.txt out/authors.str
cp script/ppcinject-patch-windows.bat out/patch.bat
7z a $OUT_NAME ./out/*
