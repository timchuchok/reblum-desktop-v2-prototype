#pragma once

#include <QWidget>

#include "core/image/ImageModel.h"
#include "core/viewport/ViewState.h"

class ImageView : public QWidget {
    Q_OBJECT
public:
    explicit ImageView(QWidget* parent = nullptr);

    void setImage(const ImageModel& model);
    void fitToView();
    void setZoom100();

    const ViewState& viewState() const { return m_state; }

signals:
    void fileDropped(const QString& path);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    bool event(QEvent* event) override;

private:
    QRectF imageRect() const;
    void applyZoom(float factor, QPointF anchor);

    ImageModel m_model;
    ViewState m_state;
    QPoint m_lastMousePos;
    bool m_panning = false;
    bool m_fitMode = true;
};
