#include "MainWindow.hpp"

#include "BoardWidget.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

using ttt::Cell;
using ttt::GameConfig;
using ttt::GameStatus;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(tr("Kolko i krzyzyk"));

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    // --- Panel konfiguracji ---
    auto* configRow = new QHBoxLayout();

    configRow->addWidget(new QLabel(tr("Rozmiar:")));
    sizeSpin_ = new QSpinBox();
    sizeSpin_->setRange(3, 20);
    sizeSpin_->setValue(3);
    configRow->addWidget(sizeSpin_);

    configRow->addWidget(new QLabel(tr("W rzedzie:")));
    winLenSpin_ = new QSpinBox();
    winLenSpin_->setRange(3, 20);
    winLenSpin_->setValue(3);
    configRow->addWidget(winLenSpin_);

    vsAiCheck_ = new QCheckBox(tr("Gra z AI (O)"));
    vsAiCheck_->setChecked(true);
    configRow->addWidget(vsAiCheck_);

    auto* newGameBtn = new QPushButton(tr("Nowa gra"));
    configRow->addWidget(newGameBtn);
    configRow->addStretch();

    layout->addLayout(configRow);

    // --- Plansza ---
    boardWidget_ = new BoardWidget();
    layout->addWidget(boardWidget_, /*stretch=*/1);

    // --- Status ---
    statusLabel_ = new QLabel();
    layout->addWidget(statusLabel_);

    setCentralWidget(central);

    connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::startNewGame);
    connect(boardWidget_, &BoardWidget::cellClicked, this, &MainWindow::onCellClicked);

    startNewGame();
}

void MainWindow::startNewGame() {
    GameConfig config;
    config.boardSize = sizeSpin_->value();
    config.winLength = winLenSpin_->value();

    game_.reset(config);
    boardWidget_->setGame(&game_);
    updateStatus();
}

void MainWindow::onCellClicked(int row, int col) {
    if (game_.isOver()) {
        return;
    }
    // W trybie z AI czlowiek gra tylko znakiem X.
    if (vsAiCheck_->isChecked() && game_.currentPlayer() != Cell::X) {
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
    // AI gra znakiem przeciwnym do czlowieka (O).
    while (!game_.isOver() && game_.currentPlayer() == Cell::O) {
        ai_.setMark(Cell::O);
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
            text = tr("Wygral X!");
            break;
        case GameStatus::OWins:
            text = tr("Wygral O!");
            break;
        case GameStatus::Draw:
            text = tr("Remis.");
            break;
    }
    statusLabel_->setText(text);
}
