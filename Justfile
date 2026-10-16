prepare:
    cmake -S . --preset default

build target="Debug":
    cmake --build --preset {{target}}

run target="Debug": (build target)
    ./build/{{target}}/TicTacToe

clean target="Debug":
    rm -rf ./build/*
