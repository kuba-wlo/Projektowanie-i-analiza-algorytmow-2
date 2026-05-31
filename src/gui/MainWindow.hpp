#pragma once

#include "AIPlayer.hpp"
#include "Game.hpp"

#include <QMainWindow>

class BoardWidget;
class QSpinBox;
class QLabel;
class QCheckBox;
class QComboBox;

// Główne okno aplikacji: panel konfiguracji (rozmiar planszy, długość
// wygrywającej linii, gra z AI) + plansza + pasek statusu.
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

    // Znaki przypisane graczowi i AI w bieżącej partii (X zawsze zaczyna).
    ttt::Cell humanMark_ = ttt::Cell::X;
    ttt::Cell aiMark_ = ttt::Cell::O;

    BoardWidget* boardWidget_ = nullptr;
    QSpinBox* sizeSpin_ = nullptr;
    QSpinBox* winLenSpin_ = nullptr;
    QComboBox* markCombo_ = nullptr;
    QComboBox* difficultyCombo_ = nullptr;
    QCheckBox* vsAiCheck_ = nullptr;
    QLabel* statusLabel_ = nullptr;
};
