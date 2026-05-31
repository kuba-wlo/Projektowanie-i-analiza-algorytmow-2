#include "GameRules.hpp"

namespace ttt {

namespace {

// Cztery kierunki linii: poziom, pion, skos \, skos /.
// Sprawdzamy tylko "do przodu" w obie strony od pola startowego.
constexpr int kDirections[4][2] = {
    {0, 1},  // ->
    {1, 0},  // v
    {1, 1},  // \ ,
    {1, -1}, // /
};

// Liczy ile pól z `mark` jest w linii przechodzącej przez (row, col)
// w kierunku (dr, dc), licząc w obie strony włącznie z polem startowym.
int countInLine(const Board& board, int row, int col, int dr, int dc, Cell mark) {
    int total = 1;

    for (int r = row + dr, c = col + dc; board.inBounds(r, c) && board.at(r, c) == mark;
         r += dr, c += dc) {
        ++total;
    }
    for (int r = row - dr, c = col - dc; board.inBounds(r, c) && board.at(r, c) == mark;
         r -= dr, c -= dc) {
        ++total;
    }
    return total;
}

} // namespace

bool GameRules::hasWonAt(const Board& board, int lastRow, int lastCol) const {
    if (!board.inBounds(lastRow, lastCol)) {
        return false;
    }
    Cell mark = board.at(lastRow, lastCol);
    if (mark == Cell::Empty) {
        return false;
    }
    for (const auto& dir : kDirections) {
        if (countInLine(board, lastRow, lastCol, dir[0], dir[1], mark) >= winLength_) {
            return true;
        }
    }
    return false;
}

bool GameRules::hasWon(const Board& board, Cell mark) const {
    if (mark == Cell::Empty) {
        return false;
    }
    for (int r = 0; r < board.size(); ++r) {
        for (int c = 0; c < board.size(); ++c) {
            if (board.at(r, c) != mark) {
                continue;
            }
            for (const auto& dir : kDirections) {
                if (countInLine(board, r, c, dir[0], dir[1], mark) >= winLength_) {
                    return true;
                }
            }
        }
    }
    return false;
}

GameStatus GameRules::status(const Board& board, Move lastMove) const {
    if (lastMove.isValid()) {
        if (hasWonAt(board, lastMove.row, lastMove.col)) {
            Cell mark = board.at(lastMove.row, lastMove.col);
            return mark == Cell::X ? GameStatus::XWins : GameStatus::OWins;
        }
    } else {
        if (hasWon(board, Cell::X)) return GameStatus::XWins;
        if (hasWon(board, Cell::O)) return GameStatus::OWins;
    }

    if (board.isFull()) {
        return GameStatus::Draw;
    }
    return GameStatus::InProgress;
}

} // namespace ttt
