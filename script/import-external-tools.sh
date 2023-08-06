#!/bin/bash
set -euxo pipefail

# We assume $PWD is the base directory of the repo
if [ $(basename "$PWD") = script ] ; then
cd ..
fi

# Import external tool configs into CLion
# Don't run this until you open the project in CLion first
mkdir -p "$PWD/.idea/tools"
cp "$PWD/script/clion-config/customTargets.xml" "$PWD/.idea"
cp "$PWD/script/clion-config/External Tools.xml" "$PWD/.idea/tools"

# Make our build script executable
chmod +x "$PWD/script/package-project.sh"
