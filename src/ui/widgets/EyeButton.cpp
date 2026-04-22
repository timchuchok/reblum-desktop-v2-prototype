#include "EyeButton.h"

#include <QPainter>

#include "ui/utils/SvgIcon.h"

EyeButton::EyeButton(QWidget* parent) : QAbstractButton(parent) {
    setCursor(Qt::PointingHandCursor);
    setFixedSize(28, 28);

    m_icon =
        svgIcon(":/icons/eye-show.svg", {20, 20}, QColor(0x74, 0x74, 0x77));
    m_iconHover =
        svgIcon(":/icons/eye-show.svg", {20, 20}, QColor(0xC9, 0xC9, 0xC9));
}

void EyeButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    if (m_hovered) {
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 38));
        p.drawRoundedRect(rect(), 4, 4);
    }
    // Centre the 20×20 icon inside the 28×28 widget
    p.drawPixmap(4, 4, (m_hovered ? m_iconHover : m_icon).pixmap({20, 20}));
}

void EyeButton::enterEvent(QEnterEvent*) {
    m_hovered = true;
    update();
}

void EyeButton::leaveEvent(QEvent*) {
    m_hovered = false;
    update();
}
