#pragma once

#include "Game.hpp"

#include <QPoint>
#include <QSize>
#include <QWidget>

// Widget rysujący planszę i obsługujący kliknięcia.
// Sam nie zna zasad - odpytuje i aktualizuje obiekt ttt::Game.
// O kliknięciu w wolne pole informuje sygnałem cellClicked.
class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr);

    // Podpięcie modelu gry (widget nie przejmuje własności wskaźnika).
    void setGame(const ttt::Game* game);

    // Sugerowany i minimalny rozmiar - plansza skaluje się do okna, ale
    // dostarcza rozsądny rozmiar startowy i dolne ograniczenie czytelności.
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    // Emitowane, gdy użytkownik kliknie w pole (row, col) na planszy.
    void cellClicked(int row, int col);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    // Geometria: bok pojedynczej komórki w pikselach.
    int cellSize() const;

    // Lewy-górny róg wyśrodkowanej, kwadratowej planszy w obrębie widgetu.
    QPoint boardOrigin() const;

    const ttt::Game* game_ = nullptr;
};
