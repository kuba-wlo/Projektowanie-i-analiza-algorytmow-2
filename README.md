# Projekt 2 – Uogólnione kółko i krzyżyk z AI (MinMax)

## Opis projektu

Projekt zawiera implementację uogólnionej gry w kółko i krzyżyk wraz z przeciwnikiem
sterowanym przez sztuczną inteligencję. Gracz definiuje **rozmiar kwadratowej planszy**
oraz **liczbę znaków w rzędzie** potrzebną do wygranej, dzięki czemu klasyczne 3×3 jest
tylko jednym z możliwych wariantów (obsługiwane są plansze do 20×20).

AI wykorzystuje algorytm **MinMax z odcinaniem alfa-beta**, ograniczeniem głębokości
przeszukiwania oraz heurystyczną oceną pozycji. Logika gry jest oddzielona od interfejsu
i zebrana we wspólnej bibliotece statycznej, z której korzystają dwie nakładki: graficzna
(**Qt 6**) oraz konsolowa (do szybkiego testowania).

## Cel projektu

Celem projektu jest:

- implementacja gry o regułach uogólnionych (dowolny rozmiar planszy i długość linii),
- zaprojektowanie przeciwnika AI w oparciu o algorytm MinMax z odcinaniem alfa-beta,
- zastosowanie heurystyki i ograniczenia głębokości, by AI pozostało grywalne także na
  dużych planszach,
- rozdzielenie logiki gry od warstwy prezentacji (wspólny rdzeń + GUI + konsola).

## Zaimplementowane algorytmy

### MinMax z odcinaniem alfa-beta

Rdzeń decyzyjny AI. Przeszukuje drzewo możliwych ruchów, naprzemiennie maksymalizując
wynik własny i minimalizując wynik przeciwnika. Odcinanie alfa-beta eliminuje gałęzie,
które nie mogą wpłynąć na wybór ruchu, co znacząco skraca przeszukiwanie. Natychmiastowa
wygrana jest premiowana wyżej niż wygrana osiągnięta głębiej w drzewie.

### Ograniczenie kandydatów i adaptacyjna głębokość

Aby drzewo nie rosło wykładniczo wraz z rozmiarem planszy, AI rozważa wyłącznie pola
sąsiadujące z już postawionymi znakami (na pustej planszy – środek). Dodatkowo głębokość
przeszukiwania jest dobierana do liczby kandydatów: małe plansze i końcówki (do ~9 wolnych
pól) przeszukiwane są w całości – dzięki czemu na 3×3 AI gra idealnie – a na dużych
planszach przeszukiwanie schodzi tym płycej, im więcej jest ruchów do rozważenia.

### Heurystyczna ocena pozycji

Gdy limit głębokości zostanie osiągnięty, pozycja jest oceniana funkcją heurystyczną.
Przesuwa ona „okno" długości równej długości wygrywającej linii po wszystkich liniach
planszy. Okna mieszane (ze znakami obu graczy) są martwe, okna z samymi znakami AI dają
punkty dodatnie, a okna z samymi znakami przeciwnika – ujemne. Wartość okna rośnie
wykładniczo wraz z liczbą znaków, co premiuje linie bliskie ukończenia.

### Losowanie spośród równorzędnych ruchów

Gdy kilka ruchów uzyskuje identyczną, najlepszą ocenę, AI losuje jeden z nich. Dzięki temu
kolejne partie nie wyglądają identycznie, mimo że gra pozostaje optymalna.

## Struktura projektu

Projekt jest podzielony na trzy główne katalogi w `src/`:

```
.
├── CMakeLists.txt        # konfiguracja całego projektu
└── src/
    ├── core/             # logika gry + AI (biblioteka ttt_core, bez GUI)
    ├── gui/              # interfejs graficzny w Qt 6 (tictactoe_gui)
    └── console/          # wersja tekstowa do testów (tictactoe_console)
```

### Najważniejsze pliki w `src/core/`

- `Types.hpp` – wspólne typy (`Cell`, `GameStatus`, `Move`, `GameConfig`)
- `Board.hpp` / `Board.cpp` – stan kwadratowej planszy N×N
- `GameRules.hpp` / `GameRules.cpp` – wykrywanie wygranej i ocena stanu gry
- `Game.hpp` / `Game.cpp` – kontroler rozgrywki (plansza + zasady + kolejność ruchów)
- `Player.hpp` – abstrakcyjny interfejs gracza
- `HumanPlayer.hpp` – gracz sterowany przez człowieka (ruch dostarcza GUI)
- `AIPlayer.hpp` / `AIPlayer.cpp` – gracz AI (MinMax, heurystyka, adaptacyjna głębokość)

### Najważniejsze pliki w `src/gui/`

- `main.cpp` – punkt wejścia aplikacji Qt
- `MainWindow.hpp` / `MainWindow.cpp` – główne okno: panel konfiguracji, plansza, status
- `BoardWidget.hpp` / `BoardWidget.cpp` – widget rysujący planszę i obsługujący kliknięcia

### Najważniejsze pliki w `src/console/`

- `main.cpp` – tekstowe menu i pętla gry do szybkiego testowania logiki oraz AI

## Jak działa program

### Warstwa logiki (rdzeń `ttt_core`)

`Game` łączy `Board` (stan pól) z `GameRules` (warunek zwycięstwa: `winLength` znaków
w rzędzie w jednym z czterech kierunków). Rdzeń nie wie nic o GUI ani o tym, kto wykonuje
ruch – operuje wyłącznie na znakach X/O. Konfiguracja jest sanityzowana, więc `winLength`
nigdy nie przekracza rozmiaru planszy.

### Wersja graficzna (Qt 6)

Po uruchomieniu okno udostępnia panel konfiguracji (rozmiar planszy, liczba znaków
w rzędzie, gra z AI), planszę oraz pasek statusu. Kliknięcie w wolne pole wykonuje ruch
gracza, po czym – w trybie z AI – ruch wylicza `AIPlayer`. `BoardWidget` jedynie odpytuje
stan z `Game` i rysuje go, a o kliknięciach informuje sygnałem `cellClicked`.

### Wersja konsolowa

Tekstowa nakładka korzysta z tej samej biblioteki `ttt_core`. Pozwala szybko sprawdzić
zasady i zachowanie AI bez uruchamiania okna Qt: rysuje planszę z indeksami pól i wczytuje
ruchy z klawiatury.

## Wykorzystane technologie

- `C++17`
- `Qt 6` (komponent Widgets) w warstwie graficznej
- `CMake` (≥ 3.21) jako system budowania
- `std::vector`, `std::mt19937` (losowanie równorzędnych ruchów)
- algorytm MinMax z odcinaniem alfa-beta
- programowanie obiektowe (interfejs `Player`, polimorfizm `HumanPlayer` / `AIPlayer`)

## Kompilacja

Wymagania: CMake ≥ 3.21, kompilator C++17, Qt 6 (komponent Widgets).

Projekt jest rozwijany przy użyciu toolchainu **MSYS2 UCRT64** (cmake, g++, ninja oraz
Qt 6 z `C:\msys64\ucrt64\bin`).

### PowerShell (Windows)

Jeśli narzędzia MSYS2 nie są w `PATH`, można dodać je tymczasowo:

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH
cmake -B build -G Ninja -DCMAKE_CXX_COMPILER=g++.exe
cmake --build build
```

### MSYS2 UCRT64

W terminalu `MSYS2 UCRT64`:

```bash
cmake -B build -G Ninja
cmake --build build
```

Powstają dwie binarki w `build/bin/`:

- `tictactoe_gui` – wersja graficzna (Qt 6),
- `tictactoe_console` – wersja tekstowa.

## Uruchamianie

### Wersja graficzna

```powershell
.\build\bin\tictactoe_gui.exe
```

### Wersja konsolowa

```powershell
.\build\bin\tictactoe_console.exe
```

## Co można ustawić z poziomu programu

- rozmiar planszy (3–20),
- liczbę znaków w rzędzie potrzebną do wygranej (3–rozmiar planszy),
- tryb gry: z drugim graczem lub z AI (człowiek gra znakiem X, AI znakiem O),
- rozpoczęcie nowej gry z bieżącą konfiguracją.

## Najważniejsze cechy projektu

- uogólnione reguły gry (dowolny rozmiar planszy i długość wygrywającej linii),
- przeciwnik AI oparty o MinMax z odcinaniem alfa-beta,
- heurystyka i adaptacyjna głębokość zapewniające grywalność także na dużych planszach,
- rdzeń logiki oddzielony od prezentacji (wspólna biblioteka + GUI + konsola),
- idealna gra na planszy 3×3 (pełne przeszukanie końcówek).

## Podsumowanie

Projekt łączy uogólnioną logikę gry w kółko i krzyżyk z przeciwnikiem AI opartym o MinMax
z odcinaniem alfa-beta, heurystyką oraz adaptacyjną głębokością przeszukiwania. Rozdzielenie
rdzenia od interfejsu pozwala korzystać z tej samej logiki zarówno w wersji graficznej
(Qt 6), jak i konsolowej, zgodnie z założeniami zadania.
