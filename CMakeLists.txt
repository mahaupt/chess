cmake_minimum_required(VERSION 3.20)

project(chess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(chess
    board.cpp
    chessAI.cpp
    enpassantflag.cpp
    figure.cpp
    figures/bishop.cpp
    figures/king.cpp
    figures/knight.cpp
    figures/pawn.cpp
    figures/queen.cpp
    figures/rook.cpp
    game.cpp
    main.cpp
    move.cpp
    movingPrefab.cpp
    point.cpp
)

target_include_directories(chess PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
