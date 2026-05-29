# Kółko i krzyżyk (Tic-Tac-Toe) — Projekt II PAA

Uogólniona gra w kółko i krzyżyk: gracz definiuje **rozmiar kwadratowej
planszy** oraz **liczbę znaków w rzędzie** potrzebną do wygranej. Przeciwnikiem
może być sztuczna inteligencja oparta o algorytm **MinMax z odcinaniem
alfa-beta**. Wersja graficzna zrealizowana w **Qt 6**.

## Struktura projektu

```
.
├── CMakeLists.txt        # konfiguracja głównego projektu
└── src/
    ├── core/             # logika gry + AI (biblioteka ttt_core, bez GUI)
    └── gui/              # interfejs graficzny w Qt 6 (tictactoe_gui)
```

## Budowanie

Wymagania: CMake ≥ 3.21, kompilator C++17, Qt 6 (komponent Widgets).

```sh
cmake -B build
cmake --build build
```

Binarka GUI: `build/bin/tictactoe_gui`.

## Stan

Projekt w budowie — szkielet powstaje przyrostowo (po jednym module na commit).
