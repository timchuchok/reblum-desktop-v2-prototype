#include "ImageView.h"

#include <QMouseEvent>
#include <QWheelEvent>

// ── Construction
// ──────────────────────────────────────────────────────────────

ImageView::ImageView(QWidget* parent) : QRhiWidget(parent) {
    setMouseTracking(true);
}

// ── Public API
// ────────────────────────────────────────────────────────────────

void ImageView::setImage(const ImageModel& model) {
    m_model = model;
    m_renderer.setImage(model.image.convertToFormat(QImage::Format_RGBA8888));
    fitToView();
}

void ImageView::setEffects(const EffectSettings& orange,
                           const EffectSettings& green) {
    m_renderer.setEffects(orange, green);
    update();
}

void ImageView::fitToView() {
    if (!m_model.isValid()) {
        update();
        return;
    }
    const QSizeF ws = size();
    const QSizeF is = m_model.size();
    m_state.zoom = std::min(ws.width() / is.width(), ws.height() / is.height());
    m_state.offset = {(ws.width() - is.width() * m_state.zoom) / 2.0f,
                      (ws.height() - is.height() * m_state.zoom) / 2.0f};
    m_fitMode = true;
    update();
}

void ImageView::setZoom100() {
    if (!m_model.isValid()) {
        return;
    }
    const QSizeF ws = size();
    const QSizeF is = m_model.size();
    m_state.zoom = 1.0f;
    m_state.offset = {(ws.width() - is.width()) / 2.0f,
                      (ws.height() - is.height()) / 2.0f};
    m_fitMode = false;
    update();
}

// ── RHI ──────────────────────────────────────────────────────────────────────

void ImageView::initialize(QRhiCommandBuffer*) {
    m_renderer.initialize(rhi(), renderTarget());
}

void ImageView::render(QRhiCommandBuffer* cb) {
    m_renderer.render(cb, renderTarget(), imageRect(),
                      static_cast<float>(devicePixelRatio()));
}

void ImageView::releaseResources() { m_renderer.releaseResources(); }

// ── Private helpers
// ───────────────────────────────────────────────────────────

QRectF ImageView::imageRect() const {
    if (!m_model.isValid()) {
        return {};
    }
    const QSizeF is = m_model.size();
    return QRectF(m_state.offset, QSizeF(is.width() * m_state.zoom,
                                         is.height() * m_state.zoom));
}

// ── Interaction
// ───────────────────────────────────────────────────────────────

void ImageView::applyZoom(float factor, QPointF anchor) {
    const float newZoom = std::clamp(m_state.zoom * factor, 0.05f, 32.0f);
    m_state.offset =
        anchor + (m_state.offset - anchor) * (newZoom / m_state.zoom);
    m_state.zoom = newZoom;
    m_fitMode = false;
    update();
}

bool ImageView::event(QEvent* event) {
    if (event->type() == QEvent::NativeGesture) {
        auto* ge = static_cast<QNativeGestureEvent*>(event);
        if (ge->gestureType() == Qt::ZoomNativeGesture) {
            applyZoom(1.0f + static_cast<float>(ge->value()),
                      ge->position().toPoint());
            return true;
        }
    }
    return QRhiWidget::event(event);
}

void ImageView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_panning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void ImageView::mouseMoveEvent(QMouseEvent* event) {
    if (!m_panning) {
        return;
    }
    const QPoint delta = event->pos() - m_lastMousePos;
    m_lastMousePos = event->pos();
    m_state.offset += QPointF(delta);
    update();
}

void ImageView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
    }
}

void ImageView::wheelEvent(QWheelEvent* event) {
    if (!m_model.isValid()) {
        return;
    }
    const int delta = event->angleDelta().y();
    if (delta == 0) {
        return;
    }
    applyZoom(delta > 0 ? 1.15f : 1.0f / 1.15f, event->position());
}

void ImageView::resizeEvent(QResizeEvent* event) {
    if (!m_model.isValid()) {
        return;
    }
    if (m_fitMode) {
        fitToView();
    } else {
        const QSize sizeDelta = event->size() - event->oldSize();
        m_state.offset +=
            QPointF(sizeDelta.width() / 2.0f, sizeDelta.height() / 2.0f);
        update();
    }
}
