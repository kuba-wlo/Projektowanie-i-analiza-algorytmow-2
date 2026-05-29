#pragma once

#include "Player.h"

namespace ttt {

// Gracz sterowany przez sztuczna inteligencje.
// Wykorzystuje algorytm MinMax z odcinaniem alfa-beta oraz ograniczeniem
// glebokosci przeszukiwania (dla wiekszych plansz pelne drzewo jest za duze,
// wiec ponizej limitu uzywamy funkcji heurystycznej).
class AIPlayer : public Player {
public:
    explicit AIPlayer(int maxDepth = 6) : maxDepth_(maxDepth) {}

    bool isHuman() const override { return false; }

    Move chooseMove(const Board& board, const GameRules& rules) override;

    int maxDepth() const { return maxDepth_; }
    void setMaxDepth(int depth) { maxDepth_ = depth; }

private:
    // Rdzen MinMaxa. Zwraca ocene pozycji z punktu widzenia gracza AI.
    // depth - aktualna glebokosc, alpha/beta - granice odciecia.
    int minimax(Board& board, const GameRules& rules, int depth, int alpha,
                int beta, Cell toMove) const;

    // Heurystyczna ocena nierozstrzygnietej pozycji (TODO: dostroic).
    int evaluate(const Board& board, const GameRules& rules) const;

    int maxDepth_;
};

} // namespace ttt
