#include "BoardWidget.hpp"

#include <QMouseEvent>
#include <QPainter>

#include <algorithm>

using ttt::Cell;

BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent) {
    // Plansza ma wypełniać dostępne miejsce - dzięki temu skaluje się wraz
    // z oknem (rozmiar komórek dobierany jest w paintEvent).
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void BoardWidget::setGame(const ttt::Game* game) {
    game_ = game;
    updateGeometry(); // zmiana liczby pól wpływa na podpowiedzi rozmiaru
    update();
}

int BoardWidget::cellSize() const {
    if (!game_) {
        return 0;
    }
    int n = game_->board().size();
    return std::min(width(), height()) / std::max(1, n);
}

QPoint BoardWidget::boardOrigin() const {
    if (!game_) {
        return {0, 0};
    }
    const int n = game_->board().size();
    const int boardPx = cellSize() * n;
    // Wyśrodkowanie kwadratowej planszy w prostokątnym widgetcie.
    return {(width() - boardPx) / 2, (height() - boardPx) / 2};
}

QSize BoardWidget::sizeHint() const {
    const int n = game_ ? game_->board().size() : 3;
    const int side = std::clamp(60 * n, 300, 600);
    return {side, side};
}

QSize BoardWidget::minimumSizeHint() const {
    const int n = game_ ? game_->board().size() : 3;
    return {18 * n, 18 * n};
}

void BoardWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(238, 241, 245)); // tlo dopasowane do okna

    if (!game_) {
        return;
    }

    const int n = game_->board().size();
    const int cs = cellSize();
    if (cs <= 0) {
        return;
    }
    const int boardPx = cs * n;
    const QPoint origin = boardOrigin();
    painter.translate(origin);

    // Plansza jako biala "karta" z delikatna ramka.
    painter.setPen(QPen(QColor(214, 219, 225), 1));
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(0, 0, boardPx, boardPx, 8, 8);

    // Podswietlenie zwycieskiej linii (pod znakami).
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(76, 175, 80, 70)); // delikatna zielen
    for (const auto& cell : game_->winningLine()) {
        painter.drawRect(cell.col * cs, cell.row * cs, cs, cs);
    }

    // Siatka.
    QPen gridPen(QColor(203, 210, 218));
    gridPen.setWidth(2);
    painter.setPen(gridPen);
    for (int i = 0; i <= n; ++i) {
        painter.drawLine(0, i * cs, boardPx, i * cs);
        painter.drawLine(i * cs, 0, i * cs, boardPx);
    }

    // Znaki (bez wypelnienia - po podswietleniu trzeba wyzerowac pedzel,
    // inaczej O rysowaloby sie wypelnione kolorem podswietlenia).
    painter.setBrush(Qt::NoBrush);
    const int margin = cs / 5;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            Cell value = game_->board().at(r, c);
            if (value == Cell::Empty) {
                continue;
            }
            QRect cellRect(c * cs + margin, r * cs + margin, cs - 2 * margin,
                           cs - 2 * margin);
            if (value == Cell::X) {
                QPen pen(QColor(60, 110, 200));
                pen.setWidth(std::max(2, cs / 12));
                painter.setPen(pen);
                painter.drawLine(cellRect.topLeft(), cellRect.bottomRight());
                painter.drawLine(cellRect.topRight(), cellRect.bottomLeft());
            } else {
                QPen pen(QColor(210, 80, 80));
                pen.setWidth(std::max(2, cs / 12));
                painter.setPen(pen);
                painter.drawEllipse(cellRect);
            }
        }
    }
}

void BoardWidget::mousePressEvent(QMouseEvent* event) {
    if (!game_) {
        return;
    }
    const int cs = cellSize();
    if (cs <= 0) {
        return;
    }
    const int n = game_->board().size();
    const QPoint origin = boardOrigin();
    const int localX = static_cast<int>(event->position().x()) - origin.x();
    const int localY = static_cast<int>(event->position().y()) - origin.y();
    if (localX < 0 || localY < 0) {
        return; // klik poza planszą (na lewo/powyżej)
    }
    const int col = localX / cs;
    const int row = localY / cs;

    if (row >= 0 && row < n && col >= 0 && col < n) {
        emit cellClicked(row, col);
    }
}
