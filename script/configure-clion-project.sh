#!/bin/bash
set -euo pipefail

# We assume $PWD is the base directory of the repo
if [ $(basename "$PWD") = script ] ; then
cd ..
fi

# Clean up so Bear can actually generate a compile_commands.json
make clean

# Generates a compilation database project file for Clion
echo "Configuring elf2rel..."
ELF2REL_BUILD=$PWD/dep/ttyd-tools/ttyd-tools/elf2rel/build
mkdir -p "$ELF2REL_BUILD"
cmake -S "$ELF2REL_BUILD/.." -B "$ELF2REL_BUILD"
bear -- make ELF2REL_BUILD="$ELF2REL_BUILD"
