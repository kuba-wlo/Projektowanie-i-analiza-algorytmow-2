#include "AIPlayer.hpp"

#include <algorithm>
#include <limits>

namespace ttt {

namespace {
constexpr int kWinScore = 1'000'000;
constexpr int kInf = std::numeric_limits<int>::max();
} // namespace

Move AIPlayer::chooseMove(const Board& board, const GameRules& rules) {
    // Kopia planszy, na ktorej AI symuluje ruchy.
    Board working = board;

    Move best;
    int bestScore = -kInf;
    int alpha = -kInf;
    int beta = kInf;

    for (int r = 0; r < working.size(); ++r) {
        for (int c = 0; c < working.size(); ++c) {
            if (!working.isEmpty(r, c)) {
                continue;
            }
            working.set(r, c, mark_);

            int score;
            if (rules.hasWonAt(working, r, c)) {
                score = kWinScore; // natychmiastowa wygrana
            } else {
                score = minimax(working, rules, 1, alpha, beta, opponent(mark_));
            }

            working.set(r, c, Cell::Empty);

            if (score > bestScore) {
                bestScore = score;
                best = {r, c};
            }
            alpha = std::max(alpha, bestScore);
        }
    }
    return best;
}

int AIPlayer::minimax(Board& board, const GameRules& rules, int depth, int alpha,
                      int beta, Cell toMove) const {
    if (board.isFull()) {
        return 0; // remis
    }
    if (depth >= maxDepth_) {
        return evaluate(board, rules);
    }

    const bool maximizing = (toMove == mark_);
    int best = maximizing ? -kInf : kInf;

    for (int r = 0; r < board.size(); ++r) {
        for (int c = 0; c < board.size(); ++c) {
            if (!board.isEmpty(r, c)) {
                continue;
            }
            board.set(r, c, toMove);

            int score;
            if (rules.hasWonAt(board, r, c)) {
                // Wygrana glebiej w drzewie jest mniej cenna niz natychmiastowa.
                score = maximizing ? (kWinScore - depth) : -(kWinScore - depth);
            } else {
                score = minimax(board, rules, depth + 1, alpha, beta, opponent(toMove));
            }

            board.set(r, c, Cell::Empty);

            if (maximizing) {
                best = std::max(best, score);
                alpha = std::max(alpha, best);
            } else {
                best = std::min(best, score);
                beta = std::min(beta, best);
            }
            if (beta <= alpha) {
                return best; // odciecie alfa-beta
            }
        }
    }
    return best;
}

int AIPlayer::evaluate(const Board& board, const GameRules& rules) const {
    // TODO: Heurystyka dla plansz, ktorych nie da sie przeszukac do konca.
    // Pomysl: zliczac "otwarte" linie dlugosci winLength, wazone liczba
    // wlasnych znakow w linii, minus to samo dla przeciwnika.
    // Na razie pozycja nierozstrzygnieta = neutralna (0).
    (void)board;
    (void)rules;
    return 0;
}

} // namespace ttt
