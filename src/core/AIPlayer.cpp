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

// Wartość „okna" (fragmentu linii długości winLength), w którym jest `count`
// znaków jednego gracza i żadnego znaku przeciwnika. Rośnie wykładniczo, by
// mocno premiować linie bliskie ukończenia (np. 2 znaki << 3 znaki).
// Ograniczona z góry, by nigdy nie pomylić jej z prawdziwą wygraną (kWinScore).
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

// Czy pole (r, c) sąsiaduje (w promieniu 1) z jakimkolwiek postawionym znakiem.
bool hasNeighbor(const Board& board, int r, int c) {
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            const int nr = r + dr;
            const int nc = c + dc;
            if (board.inBounds(nr, nc) && board.at(nr, nc) != Cell::Empty) {
                return true;
            }
        }
    }
    return false;
}
} // namespace

std::vector<Move> AIPlayer::candidateMoves(const Board& board) const {
    const int n = board.size();
    std::vector<Move> moves;

    // Pusta plansza - nie ma się wokół czego ustawiać, zaczynamy od środka.
    if (board.filledCount() == 0) {
        moves.push_back({n / 2, n / 2});
        return moves;
    }

    // Małe plansze / końcówki: bierzemy wszystkie wolne pola (pełne drzewo).
    // Wyżej ograniczamy się do pól przy już postawionych znakach.
    const bool considerAll = (n * n - board.filledCount()) <= 9;

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (!board.isEmpty(r, c)) {
                continue;
            }
            if (considerAll || hasNeighbor(board, r, c)) {
                moves.push_back({r, c});
            }
        }
    }
    return moves;
}

int AIPlayer::effectiveDepth(const Board& board, int branching) const {
    const int empties = board.size() * board.size() - board.filledCount();
    // Do ~9 wolnych pól drzewo jest małe - przeszukujemy je w całości
    // (z dokładnością do limitu trudności), dzięki czemu na 3x3 najwyższy
    // poziom gra idealnie, a niższe są słabsze.
    if (empties <= 9) {
        return std::min(empties, maxDepth_);
    }
    // Wyżej dobieramy głębokość do rozgałęziania, by drzewo nie wybuchło:
    // im więcej kandydatów, tym płycej schodzimy.
    int cap;
    if (branching <= 6) {
        cap = 6;
    } else if (branching <= 12) {
        cap = 4;
    } else if (branching <= 24) {
        cap = 3;
    } else {
        cap = 2;
    }
    return std::min(maxDepth_, cap);
}

Move AIPlayer::chooseMove(const Board& board, const GameRules& rules) {
    Board working = board;
    const std::vector<Move> candidates = candidateMoves(working);
    const int depthLimit = effectiveDepth(board, static_cast<int>(candidates.size()));

    // Zbieramy wszystkie ruchy o najlepszej ocenie, by potem wylosować jeden.
    // UWAGA: każdy ruch w korzeniu oceniamy pełnym oknem (-kInf, kInf) - bez
    // zacieśniania alpha między ruchami. Inaczej odcięcia alfa-beta zwracałyby
    // dla części ruchów jedynie oszacowanie (granicę), a nie dokładny wynik, i
    // do puli „równorzędnych" trafiałyby ruchy faktycznie gorsze (a w efekcie
    // AI losowałoby ruch przegrywający). Odcięcia w głębszych węzłach zostają.
    std::vector<Move> bestMoves;
    int bestScore = -kInf;

    for (const Move& m : candidates) {
        const int r = m.row;
        const int c = m.col;
        working.set(r, c, mark_);

        int score;
        if (rules.hasWonAt(working, r, c)) {
            score = kWinScore; // natychmiastowa wygrana
        } else {
            score = minimax(working, rules, 1, -kInf, kInf, opponent(mark_),
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
    }

    if (bestMoves.empty()) {
        return {};
    }
    // Losujemy spośród równie dobrych ruchów -> partie się różnią.
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

    for (const Move& m : candidateMoves(board)) {
        const int r = m.row;
        const int c = m.col;
        board.set(r, c, toMove);

        int score;
        if (rules.hasWonAt(board, r, c)) {
            // Wygrana głębiej w drzewie jest mniej cenna niż natychmiastowa.
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
            return best; // odcięcie alfa-beta
        }
    }
    return best;
}

int AIPlayer::evaluate(const Board& board, const GameRules& rules) const {
    // Przesuwamy „okno" długości winLength po każdej linii planszy.
    // - okno mieszane (są znaki obu graczy) jest martwe -> 0,
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
                    continue; // okno wychodzi poza planszę
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
