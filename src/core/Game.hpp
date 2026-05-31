#pragma once

#include "Board.hpp"
#include "GameRules.hpp"
#include "Types.hpp"

namespace ttt {

// Kontroler rozgrywki: łączy planszę, zasady i kolejność ruchów.
// Nie zna GUI ani konkretnych graczy - operuje na znakach (X/O).
// To tu trafia stan, który GUI odpytuje i aktualizuje.
class Game {
public:
    explicit Game(const GameConfig& config = {});

    const Board& board() const { return board_; }
    const GameRules& rules() const { return rules_; }
    const GameConfig& config() const { return config_; }

    Cell currentPlayer() const { return current_; }
    GameStatus status() const { return status_; }
    bool isOver() const { return status_ != GameStatus::InProgress; }

    // Próbuje wykonać ruch aktualnego gracza w (row, col).
    // Zwraca true, jeśli ruch był prawidłowy i został wykonany.
    bool play(int row, int col);

    // Restart gry (opcjonalnie z nową konfiguracją).
    void reset();
    void reset(const GameConfig& config);

private:
    GameConfig config_;
    Board board_;
    GameRules rules_;
    Cell current_ = Cell::X; // X zaczyna
    GameStatus status_ = GameStatus::InProgress;
};

} // namespace ttt
