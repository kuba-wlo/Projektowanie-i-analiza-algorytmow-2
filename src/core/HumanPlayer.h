#pragma once

#include "Player.h"

namespace ttt {

// Gracz sterowany przez czlowieka. Nie liczy ruchu sam - ruch dostarcza GUI
// (np. po kliknieciu w pole), wiec chooseMove zwraca ruch nieprawidlowy.
class HumanPlayer : public Player {
public:
    bool isHuman() const override { return true; }

    Move chooseMove(const Board&, const GameRules&) override { return {}; }
};

} // namespace ttt
