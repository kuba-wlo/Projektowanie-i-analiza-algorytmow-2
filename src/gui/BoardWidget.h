#pragma once

#include "Game.h"

#include <QWidget>

// Widget rysujacy plansze i obslugujacy klikniecia.
// Sam nie zna zasad - odpytuje i aktualizuje obiekt ttt::Game.
// O kliknieciu w wolne pole informuje sygnalem cellClicked.
class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr);

    // Podpiecie modelu gry (widget nie przejmuje wlasnosci wskaznika).
    void setGame(const ttt::Game* game);

signals:
    // Emitowane, gdy uzytkownik kliknie w pole (row, col) na planszy.
    void cellClicked(int row, int col);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    // Geometria: bok pojedynczej komorki w pikselach.
    int cellSize() const;

    const ttt::Game* game_ = nullptr;
};
