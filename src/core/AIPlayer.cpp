#include "AIPlayer.hpp"

#include <algorithm>
#include <limits>
#include <vector>

namespace ttt {

namespace {
constexpr int kWinScore = 1'000'000;
constexpr int kInf = std::numeric_limits<int>::max();

// Cztery kierunki linii: poziom, pion, skos \, skos /.
constexpr int kDirections[4][2] = {
    {0, 1}, {1, 0}, {1, 1}, {1, -1},
};

// Wartosc "okna" (fragmentu linii dlugosci winLength), w ktorym jest `count`
// znakow jednego gracza i zadnego znaku przeciwnika. Rosnie wykladniczo, by
// mocno premiowac linie bliskie ukonczenia (np. 2 znaki << 3 znaki).
// Ograniczona z gory, by nigdy nie pomylic jej z prawdziwa wygrana (kWinScore).
int windowValue(int count) {
    if (count <= 0) {
        return 0;
    }
    int value = 1;
    for (int i = 1; i < count; ++i) {
        value *= 10;
        if (value >= 100'000) {
            return 100'000;
        }
    }
    return value;
}

} // namespace

int AIPlayer::effectiveDepth(const Board& board) const {
    const int empties = board.size() * board.size() - board.filledCount();
    // Do ~9 wolnych pol drzewo jest male - przeszukujemy je w calosci,
    // dzieki czemu na 3x3 AI gra idealnie. Wyzej trzymamy sie limitu.
    if (empties <= 9) {
        return empties;
    }
    return maxDepth_;
}

Move AIPlayer::chooseMove(const Board& board, const GameRules& rules) {
    Board working = board;
    const int depthLimit = effectiveDepth(board);

    // Zbieramy wszystkie ruchy o najlepszej ocenie, by potem wylosowac jeden.
    std::vector<Move> bestMoves;
    int bestScore = -kInf;
    int alpha = -kInf;
    const int beta = kInf;

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
                score = minimax(working, rules, 1, alpha, beta, opponent(mark_),
                                depthLimit);
            }

            working.set(r, c, Cell::Empty);

            if (score > bestScore) {
                bestScore = score;
                bestMoves.clear();
                bestMoves.push_back({r, c});
            } else if (score == bestScore) {
                bestMoves.push_back({r, c});
            }
            alpha = std::max(alpha, bestScore);
        }
    }

    if (bestMoves.empty()) {
        return {};
    }
    // Losujemy sposrod rownie dobrych ruchow -> partie sie roznia.
    std::uniform_int_distribution<std::size_t> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(rng_)];
}

int AIPlayer::minimax(Board& board, const GameRules& rules, int depth, int alpha,
                      int beta, Cell toMove, int depthLimit) const {
    if (board.isFull()) {
        return 0; // remis
    }
    if (depth >= depthLimit) {
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
                score = minimax(board, rules, depth + 1, alpha, beta,
                                opponent(toMove), depthLimit);
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
    // Przesuwamy "okno" dlugosci winLength po kazdej linii planszy.
    // - okno mieszane (sa znaki obu graczy) jest martwe -> 0,
    // - okno z samymi moimi znakami -> punkty na plus (wg ich liczby),
    // - okno z samymi znakami przeciwnika -> punkty na minus.
    const int n = board.size();
    const int k = rules.winLength();
    const Cell me = mark_;
    const Cell foe = opponent(me);

    int score = 0;

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            for (const auto& dir : kDirections) {
                const int endR = r + (k - 1) * dir[0];
                const int endC = c + (k - 1) * dir[1];
                if (!board.inBounds(endR, endC)) {
                    continue; // okno wychodzi poza plansze
                }

                int mine = 0;
                int theirs = 0;
                for (int i = 0; i < k; ++i) {
                    Cell cell = board.at(r + i * dir[0], c + i * dir[1]);
                    if (cell == me) {
                        ++mine;
                    } else if (cell == foe) {
                        ++theirs;
                    }
                }

                if (mine > 0 && theirs == 0) {
                    score += windowValue(mine);
                } else if (theirs > 0 && mine == 0) {
                    score -= windowValue(theirs);
                }
            }
        }
    }
    return score;
}

} // namespace ttt
