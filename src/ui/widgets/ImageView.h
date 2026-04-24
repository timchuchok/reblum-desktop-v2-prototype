#pragma once

#include <QRhiWidget>

#include "core/effects/EffectSettings.h"
#include "core/image/ImageModel.h"
#include "core/viewport/ViewState.h"
#include "rendering/Renderer.h"

class QTimer;

class ImageView : public QRhiWidget {
    Q_OBJECT
public:
    explicit ImageView(QWidget* parent = nullptr);

    void setImage(const ImageModel& model);
    void setEffects(const EffectSettings& orange, const EffectSettings& green);
    void fitToView();
    void setZoom100();

    const ViewState& viewState() const { return m_state; }

signals:
    void fpsUpdated(int fps);

protected:
    void initialize(QRhiCommandBuffer* cb) override;
    void render(QRhiCommandBuffer* cb) override;
    void releaseResources() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    bool event(QEvent* event) override;

private:
    QRectF imageRect() const;
    void applyZoom(float factor, QPointF anchor);

    ImageModel m_model;
    ViewState m_state;
    Renderer m_renderer;

    QPoint m_lastMousePos;
    bool m_panning = false;
    bool m_fitMode = true;

    QTimer* m_fpsTimer = nullptr;
    int m_frameCount = 0;
};
