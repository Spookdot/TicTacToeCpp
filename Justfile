prepare target="Debug":
    cmake -S . --preset {{target}}

build target="Debug":
    cmake --build --preset {{target}}

run target="Debug": (build target)
    ./build/{{target}}/TicTacToe

clean target="Debug":
    rm -rf ./build/{{target}}/*
