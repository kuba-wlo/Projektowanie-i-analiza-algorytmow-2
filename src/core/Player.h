#pragma once

#include "Board.h"
#include "GameRules.h"
#include "Types.h"

namespace ttt {

// Abstrakcyjny interfejs gracza. Dzieki niemu Game nie musi wiedziec,
// czy ruch pochodzi od czlowieka (GUI) czy od AI (MinMax).
class Player {
public:
    virtual ~Player() = default;

    // Znak, ktorym gra ten gracz (X lub O).
    Cell mark() const { return mark_; }
    void setMark(Cell mark) { mark_ = mark; }

    // Czy gracz jest sterowany przez czlowieka (ruch przychodzi z GUI),
    // czy potrafi sam wyliczyc ruch (AI).
    virtual bool isHuman() const = 0;

    // Wybiera ruch dla AI. Dla gracza-czlowieka zwraca ruch nieprawidlowy
    // (GUI dostarcza ruch w inny sposob).
    virtual Move chooseMove(const Board& board, const GameRules& rules) = 0;

protected:
    Cell mark_ = Cell::Empty;
};

} // namespace ttt
