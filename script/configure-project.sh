#!/bin/bash
set -euo pipefail

# Generates a compilation database project file for IDEs like Clion using Bear
echo "Configuring elf2rel..."
ELF2REL_BUILD=$PWD/dep/ttyd-tools/ttyd-tools/elf2rel/build
mkdir -p $ELF2REL_BUILD
cmake -S $ELF2REL_BUILD/.. -B $ELF2REL_BUILD
bear -- make ELF2REL_BUILD=$ELF2REL_BUILD
