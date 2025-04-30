:: python tests/test-save.py

set FLAGS=-std=c++20 -Ofast

for %%f in (engine\*.cpp) do (g++ %FLAGS% -Iengine -c %%f -o build\%%~nf.o)
for %%f in (cnpy\*.cpp) do (g++ %FLAGS% -Icnpy -c %%f -o build\%%~nf.o)
for %%f in (lin\*.cpp) do (g++ %FLAGS% -Icnpy -Ilin -c %%f -o build\%%~nf.o)
for %%f in (model\*.cpp) do (g++ %FLAGS% -Icnpy -Ilin -Iengine -c %%f -o build\%%~nf.o)
for %%f in (tree\*.cpp) do (g++ %FLAGS% -Icnpy -Ilin -Iengine -Imodel -Itree -c %%f -o build\%%~nf.o)

:: g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -c tests/test-load.cpp -o build\test.o
:: g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -c tests/test-board.cpp -o build\test.o
:: g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -Itree -c tests/test-tree.cpp -o build\test.o
g++ %FLAGS% -Icnpy -Ilin -Imodel -Iengine -Itree -c tests/test-play.cpp -o build\test.o

g++ %FLAGS% build\*.o -o test

:: test

:: TODO upd global features

:: -Wl,--stack,67108864