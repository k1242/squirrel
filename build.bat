@echo off
:: python src/exec/init_rnd_weights.py

:: rd /s /q build
:: rd /s /q data/bin
:: mkdir build

set FLAGS=-std=c++20 -Ofast

:: for %%f in (src\utils\*.cpp) do (g++ %FLAGS% -Isrc\utils -c %%f -o build\%%~nf.o)
:: for %%f in (src\engine\*.cpp) do (g++ %FLAGS% -Isrc\utils -Isrc\engine -c %%f -o build\%%~nf.o)
:: for %%f in (src\mcts\*.cpp) do (g++ %FLAGS% -Isrc\utils -Isrc\engine -Isrc\mcts -c %%f -o build\%%~nf.o)
:: for %%f in (src\selfplay\*.cpp) do (g++ %FLAGS% -Isrc\utils -Isrc\engine -Isrc\mcts -Isrc\selfplay -c %%f -o build\%%~nf.o)

@echo on

:: g++ %FLAGS% -Isrc\utils -Isrc\engine -Isrc\mcts -c src\exec\play_batch.cpp -o build\main.o
:: g++ %FLAGS% build\*.o -o bin\play
:: bin\play 8 8

:: g++ %FLAGS% -Isrc\utils -Isrc\engine -Isrc\selfplay -c src\test\test_load_game.cpp -o build\main.o
:: g++ %FLAGS% build\*.o -o bin\test_load
:: bin\test_load_game 759925643690

:: g++ %FLAGS% -Isrc\utils -Isrc\engine -Isrc\selfplay -c src/exec/dump_batch.cpp -o build\main.o
:: g++ %FLAGS% build\*.o -o bin\dump

g++ %FLAGS% -Isrc\utils -Isrc\engine -Isrc\mcts -Isrc\selfplay -c src/exec/match.cpp -o build\main.o
g++ %FLAGS% build\*.o -o bin\match




:: g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -c tests/test-load.cpp -o build\test.o
:: g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -c tests/test-board.cpp -o build\test.o
:: g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -Itree -c tests/test-tree.cpp -o build\test.o
:: g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -Itree -c tests/test-load-game.cpp -o build\test.o

::g++ %FLAGS% build\*.o -o test

:: test

:: TODO upd global features

:: -Wl,--stack,67108864

:: 100 games -- 3.0s (with    model)
:: 100 games -- 1.5s (without model)

:: len(os.listdir("C:/ka1242/git/squirrel/data/bin/"))