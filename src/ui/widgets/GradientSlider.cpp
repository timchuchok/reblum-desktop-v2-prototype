#include "GradientSlider.h"

#include <QEnterEvent>
#include <QFont>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>

static constexpr int kHandleR = 7;  // half of 14 px thumb
static constexpr int kTrackH = 2;

static QPixmap loadThumb(const QString& path) {
    QSvgRenderer r(path);
    QImage img(14, 14, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p(&img);
    r.render(&p);
    return QPixmap::fromImage(img);
}

GradientSlider::GradientSlider(QColor fromColor, QColor toColor,
                               QWidget* parent)
    : QWidget(parent), m_fromColor(fromColor), m_toColor(toColor) {
    setFixedHeight(kHandleR * 2);
    setCursor(Qt::PointingHandCursor);

    m_thumbNormal = loadThumb(":/icons/slider-thumb.svg");
    m_thumbHover = loadThumb(":/icons/slider-thumb-hover.svg");
    m_thumbDisabled = loadThumb(":/icons/slider-thumb-disabled.svg");

    m_fpsTimer.setInterval(1000);
    connect(&m_fpsTimer, &QTimer::timeout, this, [this]() {
        m_lastFps = m_paintCount;
        m_paintCount = 0;
        update();
    });
    m_fpsTimer.start();
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
    const int trackLeft = kHandleR;
    const int trackRight = width() - kHandleR;
    const int trackWidth = trackRight - trackLeft;
    const int fillRight = trackLeft + static_cast<int>(m_value * trackWidth);

    const QRect trackRect(trackLeft, cy - kTrackH / 2, trackWidth, kTrackH);

    // ── Unfilled track ────────────────────────────────────────
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(40, 40, 40));
    p.drawRoundedRect(trackRect, kTrackH / 2.0, kTrackH / 2.0);

    // ── Filled portion (0 → value) ────────────────────────────
    if (m_value > 0.0f) {
        QLinearGradient grad(trackLeft, 0, fillRight, 0);
        grad.setColorAt(0.0, m_fromColor);
        grad.setColorAt(1.0, m_toColor);

        p.save();
        p.setClipRect(trackLeft, 0, fillRight - trackLeft, height());
        p.setBrush(grad);
        p.drawRoundedRect(trackRect, kTrackH / 2.0, kTrackH / 2.0);
        p.restore();
    }

    // ── Thumb ─────────────────────────────────────────────────
    const QPixmap& thumb = !isEnabled() ? m_thumbDisabled
                           : m_hovered  ? m_thumbHover
                                        : m_thumbNormal;
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.drawPixmap(fillRight - kHandleR, cy - kHandleR, thumb);

    // ── FPS overlay ───────────────────────────────────────────
    ++m_paintCount;
    p.setPen(QColor(180, 180, 180));
    p.setFont(QFont("Menlo", 8));
    p.drawText(rect().adjusted(kHandleR, 0, -kHandleR, 0),
               Qt::AlignRight | Qt::AlignVCenter,
               QString::number(m_lastFps) + " fps");
}

void GradientSlider::enterEvent(QEnterEvent*) {
    m_hovered = true;
    update();
}

void GradientSlider::leaveEvent(QEvent*) {
    m_hovered = false;
    update();
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
