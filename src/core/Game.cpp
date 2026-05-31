#include "Game.hpp"

#include <algorithm>

namespace ttt {

namespace {
// Pilnuje, by winLength nie przekraczał rozmiaru planszy.
GameConfig sanitize(GameConfig config) {
    config.boardSize = std::max(1, config.boardSize);
    config.winLength = std::clamp(config.winLength, 1, config.boardSize);
    return config;
}
} // namespace

Game::Game(const GameConfig& config)
    : config_(sanitize(config)),
      board_(config_.boardSize),
      rules_(config_.winLength) {}

bool Game::play(int row, int col) {
    if (isOver() || !board_.inBounds(row, col) || !board_.isEmpty(row, col)) {
        return false;
    }

    board_.set(row, col, current_);
    status_ = rules_.status(board_, Move{row, col});

    if (status_ == GameStatus::InProgress) {
        current_ = opponent(current_);
    }
    return true;
}

void Game::reset() {
    board_.clear();
    current_ = Cell::X;
    status_ = GameStatus::InProgress;
}

void Game::reset(const GameConfig& config) {
    config_ = sanitize(config);
    board_ = Board(config_.boardSize);
    rules_ = GameRules(config_.winLength);
    current_ = Cell::X;
    status_ = GameStatus::InProgress;
}

} // namespace ttt
