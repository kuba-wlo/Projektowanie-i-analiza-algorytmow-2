#pragma once

#include "Board.h"
#include "Types.h"

namespace ttt {

// Zasady gry: wykrywanie wygranej i ocena stanu planszy.
// Wydzielone od Board, aby latwo modyfikowac/testowac warunek zwyciestwa
// (winLength znakow w rzedzie w dowolnym z 4 kierunkow).
class GameRules {
public:
    explicit GameRules(int winLength) : winLength_(winLength) {}

    int winLength() const { return winLength_; }

    // Sprawdza, czy znak `mark` posiada gdziekolwiek na planszy liczbe
    // winLength swoich pol w jednej linii (poziomo, pionowo, po skosach).
    bool hasWon(const Board& board, Cell mark) const;

    // Optymalizacja: sprawdza wygrana przechodzac tylko przez linie
    // przecinajace ostatnio postawione pole (lastRow, lastCol).
    bool hasWonAt(const Board& board, int lastRow, int lastCol) const;

    // Zwraca ogolny status gry. lastMove pozwala uzyc szybszego sprawdzenia;
    // gdy nie podano, sprawdzane sa oba znaki na calej planszy.
    GameStatus status(const Board& board, Move lastMove = {}) const;

private:
    int winLength_;
};

} // namespace ttt
