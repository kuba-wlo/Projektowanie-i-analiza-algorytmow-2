cmake_minimum_required(VERSION 3.21)

project(TicTacToe
    VERSION 0.1.0
    DESCRIPTION "Generalized Tic-Tac-Toe (Kolko i krzyzyk) with MinMax AI"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Binaria w przewidywalnym miejscu.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Rdzeń gry (logika + AI) jako biblioteka statyczna.
add_library(ttt_core STATIC
    src/core/Board.cpp
    src/core/GameRules.cpp
    src/core/Game.cpp
    src/core/AIPlayer.cpp
)
# Katalog src/core na ścieżce include (pozwala #include "Board.hpp" itd.).
target_include_directories(ttt_core PUBLIC src/core)
target_compile_features(ttt_core PUBLIC cxx_std_17)

# Wersja konsolowa (do szybkiego testowania logiki i AI).
add_executable(tictactoe_console
    src/console/main.cpp
)
target_link_libraries(tictactoe_console PRIVATE ttt_core)

# Wersja graficzna (Qt 6 Widgets).
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(tictactoe_gui
    src/gui/main.cpp
    src/gui/MainWindow.cpp
    src/gui/BoardWidget.cpp
)

# Qt: automatyczny moc (Meta-Object Compiler) dla klas z Q_OBJECT.
set_target_properties(tictactoe_gui PROPERTIES
    AUTOMOC ON
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

target_link_libraries(tictactoe_gui PRIVATE
    ttt_core
    Qt6::Widgets
)
