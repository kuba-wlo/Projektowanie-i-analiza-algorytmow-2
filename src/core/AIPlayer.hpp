#pragma once

#include "Player.hpp"

#include <random>
#include <vector>

namespace ttt {

// Gracz sterowany przez sztuczną inteligencję.
// Wykorzystuje algorytm MinMax z odcinaniem alfa-beta oraz ograniczeniem
// głębokości przeszukiwania (dla większych plansz pełne drzewo jest za duże,
// więc poniżej limitu używamy funkcji heurystycznej).
//
// Gdy kilka ruchów ma identyczną, najlepszą ocenę, AI losuje jeden z nich -
// dzięki temu kolejne partie nie wyglądają identycznie (mimo optymalnej gry).
class AIPlayer : public Player {
public:
    explicit AIPlayer(int maxDepth = 6)
        : maxDepth_(maxDepth), rng_(std::random_device{}()) {}

    bool isHuman() const override { return false; }

    Move chooseMove(const Board& board, const GameRules& rules) override;

    int maxDepth() const { return maxDepth_; }
    void setMaxDepth(int depth) { maxDepth_ = depth; }

private:
    // Rdzeń MinMaxa. Zwraca ocenę pozycji z punktu widzenia gracza AI.
    // depth - aktualna głębokość, alpha/beta - granice odcięcia,
    // depthLimit - maksymalna głębokość dla tej tury (z effectiveDepth).
    int minimax(Board& board, const GameRules& rules, int depth, int alpha,
                int beta, Cell toMove, int depthLimit) const;

    // Heurystyczna ocena nierozstrzygniętej pozycji z punktu widzenia AI.
    // Liczy „okna" długości winLength i premiuje te, w których AI buduje
    // swoją linię, a karze te, w których przeciwnik buduje swoją.
    int evaluate(const Board& board, const GameRules& rules) const;

    // Lista pól wartych rozważenia w danej pozycji. Zamiast wszystkich wolnych
    // pól bierze tylko te sąsiadujące z już postawionymi znakami (na pustej
    // planszy - środek). Dzięki temu współczynnik rozgałęziania nie rośnie z
    // rozmiarem planszy i drzewo MinMaxa pozostaje obliczalne. Dla małych plansz
    // / końcówek zwraca wszystkie wolne pola (pełne przeszukanie).
    std::vector<Move> candidateMoves(const Board& board) const;

    // Dobiera głębokość przeszukiwania do liczby wolnych pól oraz liczby
    // kandydatów (rozgałęziania): małe plansze przeszukuje do końca, na dużych
    // schodzi tym płycej, im więcej jest ruchów do rozważenia.
    int effectiveDepth(const Board& board, int branching) const;

    int maxDepth_;
    std::mt19937 rng_; // generator do losowania spośród równie dobrych ruchów
};

} // namespace ttt
