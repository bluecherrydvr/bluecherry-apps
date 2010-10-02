#!/bin/bash

if [ -z $1 ]; then
    echo "Usage: gather_symbols.sh path/to/bundle-without-app-suffix"
    exit 1
fi

FILES="$1.app/Contents/Frameworks/*.dylib "
FILES+=`find $1.app/Contents/Frameworks/*.framework/Versions -type f -maxdepth 2`
FILES+=`find $1.app/Contents/PlugIns/ -type f \( -name '*.dylib' -or -name '*.so' \)`

# Run symbolstore for all of the libraries included in our bundle
for I in $FILES; do
    python breakpad-bin/symbolstore.py breakpad-bin/mac/dump_syms $1.symbols $I
    strip -S $I 2>/dev/null
done

rm *.stream
