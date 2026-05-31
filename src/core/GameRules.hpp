#pragma once

#include "Board.hpp"
#include "Types.hpp"

#include <vector>

namespace ttt {

// Zasady gry: wykrywanie wygranej i ocena stanu planszy.
// Wydzielone od Board, aby łatwo modyfikować/testować warunek zwycięstwa
// (winLength znaków w rzędzie w dowolnym z 4 kierunków).
class GameRules {
public:
    explicit GameRules(int winLength) : winLength_(winLength) {}

    int winLength() const { return winLength_; }

    // Sprawdza, czy znak `mark` posiada gdziekolwiek na planszy liczbę
    // winLength swoich pól w jednej linii (poziomo, pionowo, po skosach).
    bool hasWon(const Board& board, Cell mark) const;

    // Optymalizacja: sprawdza wygraną przechodząc tylko przez linie
    // przecinające ostatnio postawione pole (lastRow, lastCol).
    bool hasWonAt(const Board& board, int lastRow, int lastCol) const;

    // Zwraca pola tworzące zwycięską linię przechodzącą przez (row, col),
    // albo pustą listę, gdy w tym miejscu nie ma wygranej. Przydatne do
    // podświetlenia linii w GUI.
    std::vector<Move> winningLine(const Board& board, int row, int col) const;

    // Zwraca ogólny status gry. lastMove pozwala użyć szybszego sprawdzenia;
    // gdy nie podano, sprawdzane są oba znaki na całej planszy.
    GameStatus status(const Board& board, Move lastMove = {}) const;

private:
    int winLength_;
};

} // namespace ttt
