# Rdzen gry: logika planszy, zasady i AI.
# Budowany jako biblioteka statyczna, aby GUI (i przyszle testy)
# mogly linkowac sie do tego samego kodu.
add_library(ttt_core STATIC
    Types.hpp
    Board.hpp
    Board.cpp
    GameRules.hpp
    GameRules.cpp
    Game.hpp
    Game.cpp
    Player.hpp
    HumanPlayer.hpp
    AIPlayer.hpp
    AIPlayer.cpp
)

# Kazdy, kto linkuje ttt_core, dostaje ten katalog na sciezce include.
target_include_directories(ttt_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(ttt_core PUBLIC cxx_std_17)
