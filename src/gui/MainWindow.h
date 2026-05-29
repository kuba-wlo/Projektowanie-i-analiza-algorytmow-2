#pragma once

#include "AIPlayer.h"
#include "Game.h"

#include <QMainWindow>

class BoardWidget;
class QSpinBox;
class QLabel;
class QCheckBox;

// Glowne okno aplikacji: panel konfiguracji (rozmiar planszy, dlugosc
// wygrywajacej linii, gra z AI) + plansza + pasek statusu.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void startNewGame();
    void onCellClicked(int row, int col);

private:
    void updateStatus();
    void maybeMoveAI();

    ttt::Game game_;
    ttt::AIPlayer ai_;

    BoardWidget* boardWidget_ = nullptr;
    QSpinBox* sizeSpin_ = nullptr;
    QSpinBox* winLenSpin_ = nullptr;
    QCheckBox* vsAiCheck_ = nullptr;
    QLabel* statusLabel_ = nullptr;
};
