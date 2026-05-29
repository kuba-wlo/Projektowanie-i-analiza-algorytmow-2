#pragma once

#include "Types.hpp"

#include <vector>

namespace ttt {

// Kwadratowa plansza N x N przechowujaca znaki graczy.
// Klasa zna wylacznie stan pol - nie zna zasad wygranej (te sa w GameRules).
class Board {
public:
    explicit Board(int size = 3);

    int size() const { return size_; }

    Cell at(int row, int col) const;
    void set(int row, int col, Cell value);

    bool inBounds(int row, int col) const;
    bool isEmpty(int row, int col) const;
    bool isFull() const;

    // Liczba zajetych pol - przydatna m.in. dla AI.
    int filledCount() const { return filled_; }

    void clear();

private:
    int index(int row, int col) const { return row * size_ + col; }

    int size_;
    int filled_ = 0;
    std::vector<Cell> cells_;
};

} // namespace ttt
