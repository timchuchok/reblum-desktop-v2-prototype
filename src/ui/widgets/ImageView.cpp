#include "ImageView.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImageReader>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

ImageView::ImageView(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAcceptDrops(true);
}

void ImageView::setImage(const ImageModel& model) {
    m_model = model;
    fitToView();
}

void ImageView::fitToView() {
    if (!m_model.isValid()) {
        update();
        return;
    }

    const QSizeF widgetSize = size();
    const QSizeF imgSize = m_model.size();

    float scaleX = widgetSize.width() / imgSize.width();
    float scaleY = widgetSize.height() / imgSize.height();
    m_state.zoom = std::min(scaleX, scaleY);

    m_state.offset = {
        (widgetSize.width() - imgSize.width() * m_state.zoom) / 2.0f,
        (widgetSize.height() - imgSize.height() * m_state.zoom) / 2.0f};

    m_fitMode = true;
    update();
}

void ImageView::setZoom100() {
    if (!m_model.isValid()) {
        return;
    }

    m_fitMode = false;
    m_state.zoom = 1.0f;

    const QSizeF widgetSize = size();
    const QSizeF imgSize = m_model.size();
    m_state.offset = {(widgetSize.width() - imgSize.width()) / 2.0f,
                      (widgetSize.height() - imgSize.height()) / 2.0f};

    update();
}

QRectF ImageView::imageRect() const {
    if (!m_model.isValid()) {
        return {};
    }
    const QSizeF imgSize = m_model.size();
    return QRectF(m_state.offset, QSizeF(imgSize.width() * m_state.zoom,
                                         imgSize.height() * m_state.zoom));
}

void ImageView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), QColor("#0a0a0a"));

    if (!m_model.isValid()) {
        return;
    }

    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.drawImage(imageRect(), m_model.image);
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

    QPoint delta = event->pos() - m_lastMousePos;
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
    return QWidget::event(event);
}

void ImageView::wheelEvent(QWheelEvent* event) {
    if (!m_model.isValid()) {
        return;
    }

    const int delta = event->angleDelta().y();
    if (delta == 0) {
        return;
    }

    const float factor = delta > 0 ? 1.15f : 1.0f / 1.15f;
    applyZoom(factor, event->position());
}

void ImageView::dragEnterEvent(QDragEnterEvent* event) {
    if (!event->mimeData()->hasUrls()) {
        return;
    }
    for (const QUrl& url : event->mimeData()->urls()) {
        if (url.isLocalFile() &&
            !QImageReader::imageFormat(url.toLocalFile()).isEmpty()) {
            event->acceptProposedAction();
            return;
        }
    }
}

void ImageView::dropEvent(QDropEvent* event) {
    for (const QUrl& url : event->mimeData()->urls()) {
        if (url.isLocalFile() &&
            !QImageReader::imageFormat(url.toLocalFile()).isEmpty()) {
            emit fileDropped(url.toLocalFile());
            event->acceptProposedAction();
            return;
        }
    }
}

void ImageView::resizeEvent(QResizeEvent* event) {
    if (!m_model.isValid()) {
        return;
    }

    if (m_fitMode) {
        fitToView();
    } else {
        const QSize sizeDelta = event->size() - event->oldSize();
        const QPointF delta(sizeDelta.width() / 2.0f,
                            sizeDelta.height() / 2.0f);
        m_state.offset += delta;
        update();
    }
}
