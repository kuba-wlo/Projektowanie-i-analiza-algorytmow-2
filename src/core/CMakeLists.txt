# Rdzen gry: logika planszy, zasady i AI.
# Budowany jako biblioteka statyczna, aby GUI (i przyszle testy)
# mogly linkowac sie do tego samego kodu.
add_library(ttt_core STATIC
    Types.h
    Board.h
    Board.cpp
    GameRules.h
    GameRules.cpp
    Game.h
    Game.cpp
    Player.h
    HumanPlayer.h
    AIPlayer.h
    AIPlayer.cpp
)

# Kazdy, kto linkuje ttt_core, dostaje ten katalog na sciezce include.
target_include_directories(ttt_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(ttt_core PUBLIC cxx_std_17)
