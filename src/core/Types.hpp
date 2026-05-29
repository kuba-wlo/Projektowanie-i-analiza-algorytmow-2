#pragma once

namespace ttt {

// Stan pojedynczego pola planszy.
enum class Cell {
    Empty,
    X,
    O
};

// Wynik (stan) rozgrywki.
enum class GameStatus {
    InProgress,
    XWins,
    OWins,
    Draw
};

// Pojedynczy ruch: wiersz i kolumna (indeksowane od 0).
struct Move {
    int row = -1;
    int col = -1;

    bool isValid() const { return row >= 0 && col >= 0; }
};

// Konfiguracja gry definiowana przez gracza:
//  - boardSize : rozmiar kwadratowej planszy (boardSize x boardSize),
//  - winLength : liczba znakow w rzedzie potrzebna do wygranej.
struct GameConfig {
    int boardSize = 3;
    int winLength = 3;
};

// Zwraca przeciwny znak (X <-> O). Dla Empty zwraca Empty.
inline Cell opponent(Cell c) {
    if (c == Cell::X) return Cell::O;
    if (c == Cell::O) return Cell::X;
    return Cell::Empty;
}

} // namespace ttt
