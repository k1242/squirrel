#!/bin/bash

FLAGS="-std=c++20 -Ofast"

mkdir -p build
mkdir -p bin


for f in src/utils/*.cpp; do
    g++ $FLAGS -Isrc/utils -c "$f" -o "build/$(basename "${f%.cpp}").o"
done

for f in src/engine/*.cpp; do
    g++ $FLAGS -Isrc/utils -Isrc/engine -c "$f" -o "build/$(basename "${f%.cpp}").o"
done

for f in src/mcts/*.cpp; do
    g++ $FLAGS -Isrc/utils -Isrc/engine -Isrc/mcts -c "$f" -o "build/$(basename "${f%.cpp}").o"
done

for f in src/selfplay/*.cpp; do
    g++ $FLAGS -Isrc/utils -Isrc/engine -Isrc/mcts -Isrc/selfplay -c "$f" -o "build/$(basename "${f%.cpp}").o"
done


g++ $FLAGS -Isrc/utils -Isrc/engine -Isrc/mcts -c src/exec/play_batch.cpp -o build/main.o
g++ $FLAGS build/*.o -o bin/play

g++ $FLAGS -Isrc/utils -Isrc/engine -Isrc/selfplay -c src/test/test_load_game.cpp -o build/main.o
g++ $FLAGS build/*.o -o bin/test_load

g++ $FLAGS -Isrc/utils -Isrc/engine -Isrc/selfplay -c src/exec/dump_batch.cpp -o build/main.o
g++ $FLAGS build/*.o -o bin/dump

g++ $FLAGS -Isrc/utils -Isrc/engine -Isrc/mcts -Isrc/selfplay -c src/exec/match.cpp -o build/main.o
g++ $FLAGS build/*.o -o bin/match
