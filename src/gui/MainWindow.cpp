#include "MainWindow.hpp"

#include "BoardWidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

using ttt::Cell;
using ttt::GameConfig;
using ttt::GameStatus;
using ttt::opponent;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("Kółko i krzyżyk"));

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    // Panel konfiguracji.
    auto* configRow = new QHBoxLayout();

    configRow->addWidget(new QLabel(tr("Rozmiar:")));
    sizeSpin_ = new QSpinBox();
    sizeSpin_->setRange(3, 20);
    sizeSpin_->setValue(3);
    configRow->addWidget(sizeSpin_);

    configRow->addWidget(new QLabel(tr("W rzędzie:")));
    winLenSpin_ = new QSpinBox();
    winLenSpin_->setRange(3, 20);
    winLenSpin_->setValue(3);
    configRow->addWidget(winLenSpin_);

    configRow->addWidget(new QLabel(tr("Grasz:")));
    markCombo_ = new QComboBox();
    markCombo_->addItem(tr("X"));
    markCombo_->addItem(tr("O"));
    configRow->addWidget(markCombo_);

    configRow->addWidget(new QLabel(tr("Poziom:")));
    difficultyCombo_ = new QComboBox();
    difficultyCombo_->addItem(tr("Łatwy"));
    difficultyCombo_->addItem(tr("Średni"));
    difficultyCombo_->addItem(tr("Trudny"));
    difficultyCombo_->setCurrentIndex(2);
    configRow->addWidget(difficultyCombo_);

    vsAiCheck_ = new QCheckBox(tr("Gra z AI"));
    vsAiCheck_->setChecked(true);
    configRow->addWidget(vsAiCheck_);

    auto* newGameBtn = new QPushButton(tr("Nowa gra"));
    configRow->addWidget(newGameBtn);
    configRow->addStretch();

    layout->addLayout(configRow);

    // Plansza wypełnia dostępne miejsce i skaluje się wraz z oknem.
    boardWidget_ = new BoardWidget();
    layout->addWidget(boardWidget_, /*stretch=*/1);

    // Pasek statusu.
    statusLabel_ = new QLabel();
    layout->addWidget(statusLabel_);

    setCentralWidget(central);
    resize(640, 720); // rozmiar startowy - dalej okno (i plansza) skalują się dowolnie

    connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::startNewGame);
    connect(boardWidget_, &BoardWidget::cellClicked, this, &MainWindow::onCellClicked);

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
