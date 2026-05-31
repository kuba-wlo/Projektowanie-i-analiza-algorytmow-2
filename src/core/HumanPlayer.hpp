#pragma once

#include "Player.hpp"

namespace ttt {

// Gracz sterowany przez człowieka. Nie liczy ruchu sam - ruch dostarcza GUI
// (np. po kliknięciu w pole), więc chooseMove zwraca ruch nieprawidłowy.
class HumanPlayer : public Player {
public:
    bool isHuman() const override { return true; }

    Move chooseMove(const Board&, const GameRules&) override { return {}; }
};

} // namespace ttt
