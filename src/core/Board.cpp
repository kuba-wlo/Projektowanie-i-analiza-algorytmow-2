#include "Board.h"

#include <stdexcept>

namespace ttt {

Board::Board(int size)
    : size_(size), cells_(static_cast<std::size_t>(size) * size, Cell::Empty) {
    if (size <= 0) {
        throw std::invalid_argument("Board size must be positive");
    }
}

bool Board::inBounds(int row, int col) const {
    return row >= 0 && row < size_ && col >= 0 && col < size_;
}

Cell Board::at(int row, int col) const {
    if (!inBounds(row, col)) {
        throw std::out_of_range("Board::at - index out of range");
    }
    return cells_[index(row, col)];
}

void Board::set(int row, int col, Cell value) {
    if (!inBounds(row, col)) {
        throw std::out_of_range("Board::set - index out of range");
    }
    Cell& slot = cells_[index(row, col)];
    if (slot == Cell::Empty && value != Cell::Empty) {
        ++filled_;
    } else if (slot != Cell::Empty && value == Cell::Empty) {
        --filled_;
    }
    slot = value;
}

bool Board::isEmpty(int row, int col) const {
    return at(row, col) == Cell::Empty;
}

bool Board::isFull() const {
    return filled_ == size_ * size_;
}

void Board::clear() {
    std::fill(cells_.begin(), cells_.end(), Cell::Empty);
    filled_ = 0;
}

} // namespace ttt
