cmake_minimum_required(VERSION 3.21)

project(TicTacToe
    VERSION 0.1.0
    DESCRIPTION "Generalized Tic-Tac-Toe (Kolko i krzyzyk) with MinMax AI"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put binaries in a predictable place.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Caly kod zrodlowy (logika gry + GUI) znajduje sie w src/.
add_subdirectory(src)
