#include "GradientSlider.h"

#include <QMouseEvent>
#include <QPainter>

static constexpr int kHandleR = 7;
static constexpr int kTrackH = 4;

GradientSlider::GradientSlider(QColor color, QWidget* parent)
    : QWidget(parent), m_color(color) {
    setFixedHeight(kHandleR * 2);
    setCursor(Qt::PointingHandCursor);
}

void GradientSlider::setValue(float value) {
    value = std::clamp(value, 0.0f, 1.0f);
    if (qFuzzyCompare(m_value, value)) {
        return;
    }
    m_value = value;
    update();
    emit valueChanged(m_value);
}

float GradientSlider::valueFromX(int x) const {
    const int range = width() - 2 * kHandleR;
    if (range <= 0) {
        return 0.0f;
    }
    return std::clamp(static_cast<float>(x - kHandleR) / range, 0.0f, 1.0f);
}

void GradientSlider::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int cy = height() / 2;
    const int range = width() - 2 * kHandleR;
    const int handleX = kHandleR + static_cast<int>(m_value * range);

    // Track gradient
    QLinearGradient grad(kHandleR, 0, width() - kHandleR, 0);
    grad.setColorAt(0.0, QColor(50, 50, 50));
    grad.setColorAt(1.0, m_color);

    p.setPen(Qt::NoPen);
    p.setBrush(grad);
    p.drawRoundedRect(kHandleR, cy - kTrackH / 2, width() - 2 * kHandleR,
                      kTrackH, kTrackH / 2.0, kTrackH / 2.0);

    // Handle
    p.setBrush(QColor(210, 210, 210));
    p.drawEllipse(QPoint(handleX, cy), kHandleR - 2, kHandleR - 2);
}

void GradientSlider::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        setValue(valueFromX(event->pos().x()));
    }
}

void GradientSlider::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        setValue(valueFromX(event->pos().x()));
    }
}

void GradientSlider::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}
