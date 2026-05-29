#pragma once

#include "Player.hpp"

#include <random>

namespace ttt {

// Gracz sterowany przez sztuczna inteligencje.
// Wykorzystuje algorytm MinMax z odcinaniem alfa-beta oraz ograniczeniem
// glebokosci przeszukiwania (dla wiekszych plansz pelne drzewo jest za duze,
// wiec ponizej limitu uzywamy funkcji heurystycznej).
//
// Gdy kilka ruchow ma identyczna, najlepsza ocene, AI losuje jeden z nich -
// dzieki temu kolejne partie nie wygladaja identycznie (mimo optymalnej gry).
class AIPlayer : public Player {
public:
    explicit AIPlayer(int maxDepth = 6)
        : maxDepth_(maxDepth), rng_(std::random_device{}()) {}

    bool isHuman() const override { return false; }

    Move chooseMove(const Board& board, const GameRules& rules) override;

    int maxDepth() const { return maxDepth_; }
    void setMaxDepth(int depth) { maxDepth_ = depth; }

private:
    // Rdzen MinMaxa. Zwraca ocene pozycji z punktu widzenia gracza AI.
    // depth - aktualna glebokosc, alpha/beta - granice odciecia,
    // depthLimit - maksymalna glebokosc dla tej tury (z effectiveDepth).
    int minimax(Board& board, const GameRules& rules, int depth, int alpha,
                int beta, Cell toMove, int depthLimit) const;

    // Heurystyczna ocena nierozstrzygnietej pozycji z punktu widzenia AI.
    // Liczy "okna" dlugosci winLength i premiuje te, w ktorych AI buduje
    // swoja linie, a karze te, w ktorych przeciwnik buduje swoja.
    int evaluate(const Board& board, const GameRules& rules) const;

    // Dobiera glebokosc przeszukiwania do liczby wolnych pol:
    // male plansze przeszukuje do konca, duze - tylko do maxDepth_.
    int effectiveDepth(const Board& board) const;

    int maxDepth_;
    std::mt19937 rng_; // generator do losowania sposrod rownie dobrych ruchow
};

} // namespace ttt
