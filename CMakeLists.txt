cmake_minimum_required(VERSION 3.19)
project(
    TicTacToe
    VERSION 0.1
    LANGUAGES CXX
)

include(cmake/CPM.cmake)

CPMAddPackage("gh:raysan5/raylib#6.0")
CPMAddPackage(
    URI "gh:RobLoach/raylib-cpp@6.0.3"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC raylib 
    PUBLIC raylib_cpp
)
