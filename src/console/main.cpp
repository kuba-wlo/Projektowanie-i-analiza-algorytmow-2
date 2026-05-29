// Wersja konsolowa gry w kolko i krzyzyk.
// Korzysta z tej samej logiki co GUI (biblioteka ttt_core) - sluzy do
// szybkiego testowania zasad i AI bez uruchamiania okna Qt.

#include "AIPlayer.hpp"
#include "Game.hpp"
#include "Types.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

using namespace ttt;

namespace {

char glyph(Cell c) {
    switch (c) {
        case Cell::X: return 'X';
        case Cell::O: return 'O';
        default: return '.';
    }
}

// Rysuje plansze z indeksami wierszy/kolumn, by ulatwic podawanie ruchow.
void printBoard(const Board& board) {
    const int n = board.size();

    std::cout << "\n    ";
    for (int c = 0; c < n; ++c) {
        std::cout << c % 10 << ' ';
    }
    std::cout << "\n   ";
    for (int c = 0; c < n; ++c) {
        std::cout << "--";
    }
    std::cout << '\n';

    for (int r = 0; r < n; ++r) {
        std::cout << (r % 10) << " | ";
        for (int c = 0; c < n; ++c) {
            std::cout << glyph(board.at(r, c)) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

// Wczytuje liczbe calkowita z zakresu [min, max], ponawia przy bledzie.
// Przy koncu wejscia (EOF) konczy program, by nie zapetlic sie w nieskonczonosc.
int readInt(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= min && value <= max) {
                return value;
            }
        } else if (std::cin.eof()) {
            std::cout << "\nKoniec wejscia - zamykam gre.\n";
            std::exit(0);
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Nieprawidlowa wartosc (oczekiwano " << min << ".." << max
                  << ").\n";
    }
}

void printResult(GameStatus status) {
    switch (status) {
        case GameStatus::XWins: std::cout << "Wygral gracz X!\n"; break;
        case GameStatus::OWins: std::cout << "Wygral gracz O!\n"; break;
        case GameStatus::Draw: std::cout << "Remis.\n"; break;
        default: break;
    }
}

} // namespace

int main() {
    std::cout << "=== Kolko i krzyzyk (konsola) ===\n";

    const int size = readInt("Rozmiar planszy (3-20): ", 3, 20);
    const int winLen = readInt("Znakow w rzedzie do wygranej (3-" +
                                   std::to_string(size) + "): ",
                               3, size);
    const bool vsAi = readInt("Przeciwnik: 1 = czlowiek, 2 = AI: ", 1, 2) == 2;

    Game game(GameConfig{size, winLen});
    AIPlayer ai;
    ai.setMark(Cell::O); // czlowiek gra X, AI gra O

    while (!game.isOver()) {
        printBoard(game.board());
        const Cell turn = game.currentPlayer();

        if (vsAi && turn == Cell::O) {
            std::cout << "Ruch AI (O)...\n";
            Move m = ai.chooseMove(game.board(), game.rules());
            game.play(m.row, m.col);
        } else {
            std::cout << "Ruch gracza " << glyph(turn) << '\n';
            const int row = readInt("  wiersz: ", 0, size - 1);
            const int col = readInt("  kolumna: ", 0, size - 1);
            if (!game.play(row, col)) {
                std::cout << "  Pole zajete lub poza plansza - sprobuj ponownie.\n";
            }
        }
    }

    printBoard(game.board());
    printResult(game.status());
    return 0;
}
