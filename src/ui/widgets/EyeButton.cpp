#include "EyeButton.h"

#include <QPainter>

EyeButton::EyeButton(QWidget* parent) : QAbstractButton(parent) {
    setCheckable(true);
    setChecked(true);
    setCursor(Qt::PointingHandCursor);
    setFixedSize(20, 20);
}

void EyeButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor color =
        isChecked() ? QColor(220, 220, 220) : QColor(70, 70, 70);
    const float r = qMin(width(), height()) / 2.0f - 2.0f;
    const QPointF center(width() / 2.0f, height() / 2.0f);

    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawEllipse(center, r, r);
}
