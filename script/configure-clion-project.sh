#!/bin/bash
set -euo pipefail

# Import external tool configs into Clion
mkdir -p $PWD/.idea/tools
cp "$PWD/script/clion-config/customTargets.xml" "$PWD/.idea"
cp "$PWD/script/clion-config/External Tools.xml" "$PWD/.idea/tools"

# Generates a compilation database project file for Clion
echo "Configuring elf2rel..."
ELF2REL_BUILD=$PWD/dep/ttyd-tools/ttyd-tools/elf2rel/build
mkdir -p $ELF2REL_BUILD
cmake -S $ELF2REL_BUILD/.. -B $ELF2REL_BUILD
bear -- make ELF2REL_BUILD=$ELF2REL_BUILD
