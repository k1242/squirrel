#!/bin/bash
set -e

FLAGS="-std=c++20 -Ofast"
OUTDIR="build"

mkdir -p "$OUTDIR"

for f in engine/*.cpp; do
    g++ $FLAGS -Iengine -c "$f" -o "$OUTDIR/$(basename "${f%.cpp}.o")"
done

for f in cnpy/*.cpp; do
    g++ $FLAGS -Icnpy -c "$f" -o "$OUTDIR/$(basename "${f%.cpp}.o")"
done

for f in lin/*.cpp; do
    g++ $FLAGS -Ilin -Icnpy -c "$f" -o "$OUTDIR/$(basename "${f%.cpp}.o")"
done

for f in model/*.cpp; do
    g++ $FLAGS -Imodel -Iengine -Ilin -Icnpy -c "$f" -o "$OUTDIR/$(basename "${f%.cpp}.o")"
done

for f in tree/*.cpp; do
    g++ $FLAGS -Itree -Imodel -Iengine -Ilin -Icnpy -c "$f" -o "$OUTDIR/$(basename "${f%.cpp}.o")"
done

g++ $FLAGS -Iengine -Ilin -Imodel -Icnpy -Itree -c tests/test-play.cpp -o "$OUTDIR/test.o"

g++ $FLAGS "$OUTDIR"/*.o -o test
