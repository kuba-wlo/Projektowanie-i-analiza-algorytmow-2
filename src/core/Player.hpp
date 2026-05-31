#pragma once

#include "Board.hpp"
#include "GameRules.hpp"
#include "Types.hpp"

namespace ttt {

// Abstrakcyjny interfejs gracza. Dzięki niemu Game nie musi wiedzieć,
// czy ruch pochodzi od człowieka (GUI) czy od AI (MinMax).
class Player {
public:
    virtual ~Player() = default;

    // Znak, którym gra ten gracz (X lub O).
    Cell mark() const { return mark_; }
    void setMark(Cell mark) { mark_ = mark; }

    // Czy gracz jest sterowany przez człowieka (ruch przychodzi z GUI),
    // czy potrafi sam wyliczyć ruch (AI).
    virtual bool isHuman() const = 0;

    // Wybiera ruch dla AI. Dla gracza-człowieka zwraca ruch nieprawidłowy
    // (GUI dostarcza ruch w inny sposób).
    virtual Move chooseMove(const Board& board, const GameRules& rules) = 0;

protected:
    Cell mark_ = Cell::Empty;
};

} // namespace ttt
