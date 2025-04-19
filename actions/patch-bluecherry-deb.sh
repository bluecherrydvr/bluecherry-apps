#!/bin/bash

set -e

INPUT_DEB=$1
OUTPUT_DEB=${2:-patched_${INPUT_DEB}}

if [[ -z "$INPUT_DEB" || ! -f "$INPUT_DEB" ]]; then
    echo "Usage: $0 <bluecherry.deb> [output.deb]"
    exit 1
fi

# Create working directory
WORKDIR=$(mktemp -d)
echo "Unpacking $INPUT_DEB..."
dpkg-deb -R "$INPUT_DEB" "$WORKDIR"

POSTINST="$WORKDIR/DEBIAN/postinst"
if [[ -f "$POSTINST" ]]; then
    echo "Patching postinst..."
    sed -i 's/invoke-rc.d /invoke-rc.d /g; s/invoke-rc.d .*/& || true/' "$POSTINST"
else
    echo "No postinst script found! Skipping."
fi

echo "Rebuilding .deb as $OUTPUT_DEB..."
dpkg-deb -b "$WORKDIR" "$OUTPUT_DEB"

# Clean up
rm -rf "$WORKDIR"

echo "Done."
