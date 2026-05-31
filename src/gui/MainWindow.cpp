#include "MainWindow.hpp"

#include "BoardWidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

namespace {
// Arkusz stylów (QSS) - nowoczesny, jasny wygląd okna.
constexpr auto kStyleSheet = R"(
QWidget { background: #eef1f5; color: #2b2f36; font-size: 13px; }
QLabel#heading { font-size: 26px; font-weight: bold; color: #1f2933; }
QLabel#subtitle { color: #6b7280; font-size: 12px; }
QLabel#status { font-size: 17px; font-weight: 600; padding: 8px 2px; color: #1f2933; }
QGroupBox {
    background: #ffffff; border: 1px solid #d6dbe1; border-radius: 10px;
    margin-top: 14px; padding: 12px; font-weight: 600;
}
QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; color: #4b5563; }
QLabel#field { color: #6b7280; font-size: 11px; font-weight: 600; }
QSpinBox, QComboBox {
    background: #ffffff; border: 1px solid #cbd2da; border-radius: 6px;
    padding: 5px 8px; min-height: 22px;
}
QSpinBox:focus, QComboBox:focus { border: 1px solid #3b82f6; }
QComboBox:disabled, QSpinBox:disabled { background: #f1f3f6; color: #9aa3ad; }
QPushButton {
    background: #3b82f6; color: #ffffff; border: none; border-radius: 8px;
    padding: 9px 20px; font-weight: 600;
}
QPushButton:hover { background: #2f6fe0; }
QPushButton:pressed { background: #2a63c8; }
QCheckBox { spacing: 6px; font-weight: 600; }
)";

// Etykieta nad polem formularza (drobny, szary nagłówek kolumny).
QLabel* fieldLabel(const QString& text) {
    auto* label = new QLabel(text);
    label->setObjectName("field");
    return label;
}
} // namespace

using ttt::Cell;
using ttt::GameConfig;
using ttt::GameStatus;
using ttt::opponent;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("Kółko i krzyżyk"));

    auto* central = new QWidget(this);
    central->setStyleSheet(kStyleSheet);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(12);

    // Nagłówek.
    auto* heading = new QLabel(tr("Kółko i krzyżyk"));
    heading->setObjectName("heading");
    layout->addWidget(heading);
    auto* subtitle = new QLabel(
        tr("Ustaw rozmiar planszy, liczbę znaków w rzędzie i przeciwnika."));
    subtitle->setObjectName("subtitle");
    layout->addWidget(subtitle);

    // Panel ustawień: każda kontrolka w kolumnie z etykietą u góry.
    auto* configGroup = new QGroupBox(tr("Ustawienia gry"));
    auto* grid = new QGridLayout(configGroup);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(6);

    sizeSpin_ = new QSpinBox();
    sizeSpin_->setRange(3, 20);
    sizeSpin_->setValue(3);

    winLenSpin_ = new QSpinBox();
    winLenSpin_->setRange(3, sizeSpin_->value()); // nie więcej niż rozmiar planszy
    winLenSpin_->setValue(3);

    markCombo_ = new QComboBox();
    markCombo_->addItem(tr("X"));
    markCombo_->addItem(tr("O"));

    difficultyCombo_ = new QComboBox();
    difficultyCombo_->addItem(tr("Łatwy"));
    difficultyCombo_->addItem(tr("Średni"));
    difficultyCombo_->addItem(tr("Trudny"));
    difficultyCombo_->setCurrentIndex(2);

    grid->addWidget(fieldLabel(tr("Rozmiar planszy")), 0, 0);
    grid->addWidget(sizeSpin_, 1, 0);
    grid->addWidget(fieldLabel(tr("Znaki w rzędzie")), 0, 1);
    grid->addWidget(winLenSpin_, 1, 1);
    grid->addWidget(fieldLabel(tr("Twój znak")), 0, 2);
    grid->addWidget(markCombo_, 1, 2);
    grid->addWidget(fieldLabel(tr("Poziom AI")), 0, 3);
    grid->addWidget(difficultyCombo_, 1, 3);

    vsAiCheck_ = new QCheckBox(tr("Gra z AI"));
    vsAiCheck_->setChecked(true);
    grid->addWidget(vsAiCheck_, 1, 4);

    grid->setColumnStretch(5, 1); // dosuwa kontrolki do lewej, przycisk do prawej
    auto* newGameBtn = new QPushButton(tr("Nowa gra"));
    grid->addWidget(newGameBtn, 1, 6);

    layout->addWidget(configGroup);

    // Plansza wypełnia dostępne miejsce i skaluje się wraz z oknem.
    boardWidget_ = new BoardWidget();
    layout->addWidget(boardWidget_, /*stretch=*/1);

    // Pasek statusu.
    statusLabel_ = new QLabel();
    statusLabel_->setObjectName("status");
    layout->addWidget(statusLabel_);

    setCentralWidget(central);
    resize(680, 760); // rozmiar startowy - dalej okno (i plansza) skalują się dowolnie

    connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::startNewGame);
    connect(boardWidget_, &BoardWidget::cellClicked, this, &MainWindow::onCellClicked);

    // Liczba znaków w rzędzie nie może przekraczać rozmiaru planszy.
    connect(sizeSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [this](int size) { winLenSpin_->setMaximum(size); });

    // Wybor znaku i poziomu ma sens tylko w grze z AI.
    auto syncAiControls = [this] {
        const bool vsAi = vsAiCheck_->isChecked();
        markCombo_->setEnabled(vsAi);
        difficultyCombo_->setEnabled(vsAi);
    };
    connect(vsAiCheck_, &QCheckBox::toggled, this, syncAiControls);
    syncAiControls();

    startNewGame();
}

void MainWindow::startNewGame() {
    GameConfig config;
    config.boardSize = sizeSpin_->value();
    config.winLength = winLenSpin_->value();

    game_.reset(config);

    // Znak gracza (X zawsze zaczyna) i znak AI.
    humanMark_ = (markCombo_->currentIndex() == 0) ? Cell::X : Cell::O;
    aiMark_ = opponent(humanMark_);

    // Poziom trudnosci -> maksymalna glebokosc przeszukiwania AI.
    static const int kDepthByLevel[] = {1, 4, 9}; // latwy, sredni, trudny
    ai_.setMaxDepth(kDepthByLevel[difficultyCombo_->currentIndex()]);

    boardWidget_->setGame(&game_);
    updateStatus();

    // Gdy gracz wybrał O, ruch zaczyna AI (gra znakiem X).
    maybeMoveAI();
}

void MainWindow::onCellClicked(int row, int col) {
    if (game_.isOver()) {
        return;
    }
    // W trybie z AI człowiek gra tylko swoim znakiem.
    if (vsAiCheck_->isChecked() && game_.currentPlayer() != humanMark_) {
        return;
    }

    if (game_.play(row, col)) {
        boardWidget_->update();
        updateStatus();
        maybeMoveAI();
    }
}

void MainWindow::maybeMoveAI() {
    if (!vsAiCheck_->isChecked() || game_.isOver()) {
        return;
    }
    // AI gra swoim znakiem (przeciwnym do gracza).
    while (!game_.isOver() && game_.currentPlayer() == aiMark_) {
        ai_.setMark(aiMark_);
        ttt::Move move = ai_.chooseMove(game_.board(), game_.rules());
        if (!move.isValid()) {
            break;
        }
        game_.play(move.row, move.col);
        boardWidget_->update();
        updateStatus();
    }
}

void MainWindow::updateStatus() {
    QString text;
    switch (game_.status()) {
        case GameStatus::InProgress:
            text = (game_.currentPlayer() == Cell::X) ? tr("Ruch: X") : tr("Ruch: O");
            break;
        case GameStatus::XWins:
            text = tr("Wygrał X!");
            break;
        case GameStatus::OWins:
            text = tr("Wygrał O!");
            break;
        case GameStatus::Draw:
            text = tr("Remis.");
            break;
    }
    statusLabel_->setText(text);
}
