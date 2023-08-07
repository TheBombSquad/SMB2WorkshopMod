#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# To use this script, update the path variables below with valid paths, then run the script
# If you are using CLion, rename this to `package-project.sh`, then you will be able to build it in CLion
# Also if you're using CLion: Make sure you run `configure-clion-project.sh` first

# Path to PPCInject - in the future, this should be compiled alongside WSMod
# Found here: https://github.com/tuckergs/ppc-inject
PPCINJECT=PATH/TO/YOUR/PPCInject

# Path to your main.dol file from Super Monkey Ball 2
# You can extract this from an image of your legally obtained copy of Super Monkey Ball 2
DOL_SRC=PATH/TO/YOUR/main.dol

# Destination path for the patched main.dol file
# Can be the same path as DOL_SRC, but this will replace the main.dol file
DOL_DEST=PATH/TO/YOUR/main.dol

# Path to the REL loader code, which is what main.dol is patched with
# Included in the repo
RELLOADER_SRC=relloader/iso-rel-loader-us.asm

# Destination of the REL file
# Ideally you want the path to an extracted root of Super Monkey Ball 2
# You'll probably want to replace mkb2.rel_sample.rel
# If you change the file name, you also need to modify iso-rel-loader-us.asm accordingly
REL_DEST=PATH/TO/YOUR/mkb2.rel_sample.rel

# Pass -i to this script to inject the main.dol file
if [[ "${1:-}" == "-i" ]]; then
    echo 'Patching main.dol with ppcinject'
    "$PPCINJECT" "$DOL_SRC" "$DOL_DEST" "$RELLOADER_SRC"
fi

make -j$(nproc)

echo 'Copying REL to' $REL_DEST
cp mkb2.rel_sample.rel $REL_DEST
