#include "RightPanel.h"

#include <QFrame>
#include <QTimer>
#include <QVBoxLayout>

#include "EffectPanel.h"
#include "app/EffectsController.h"

RightPanel::RightPanel(EffectsController* controller, QWidget* parent)
    : QWidget(parent) {
    setObjectName("RightPanel");
    setFixedWidth(294);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 16, 12, 16);
    layout->setSpacing(0);

    m_orange = new EffectPanel(EffectType::Orange, this);

    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName("PanelSeparator");

    m_green = new EffectPanel(EffectType::Green, this);

    // ── Progress bar ──────────────────────────────────────
    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("EffectProgress");
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(3);
    m_progressBar->setTextVisible(false);

    m_progressAnim = new QPropertyAnimation(m_progressBar, "value", this);
    m_progressAnim->setDuration(2500);
    m_progressAnim->setStartValue(0);
    m_progressAnim->setEndValue(100);
    m_progressAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_progressAnim, &QPropertyAnimation::finished, this, [this]() {
        QTimer::singleShot(200, this, [this]() { m_progressBar->setValue(0); });
    });

    layout->addWidget(m_orange);
    layout->addSpacing(16);
    layout->addWidget(sep);
    layout->addSpacing(16);
    layout->addWidget(m_green);
    layout->addSpacing(16);
    layout->addWidget(m_progressBar);
    layout->addStretch();

    connect(m_orange, &EffectPanel::opacityChanged, controller,
            &EffectsController::setOrangeOpacity);
    connect(m_orange, &EffectPanel::thresholdChanged, controller,
            &EffectsController::setOrangeThreshold);
    connect(m_orange, &EffectPanel::enabledChanged, controller,
            &EffectsController::setOrangeEnabled);
    connect(m_green, &EffectPanel::opacityChanged, controller,
            &EffectsController::setGreenOpacity);
    connect(m_green, &EffectPanel::thresholdChanged, controller,
            &EffectsController::setGreenThreshold);
    connect(m_green, &EffectPanel::enabledChanged, controller,
            &EffectsController::setGreenEnabled);

    // trigger progress on any slider change
    auto trigger = [this](float) { triggerProgress(); };
    connect(m_orange, &EffectPanel::opacityChanged, this, trigger);
    connect(m_orange, &EffectPanel::thresholdChanged, this, trigger);
    connect(m_green, &EffectPanel::opacityChanged, this, trigger);
    connect(m_green, &EffectPanel::thresholdChanged, this, trigger);
}

void RightPanel::triggerProgress() {
    m_progressAnim->stop();
    m_progressBar->setValue(0);
    m_progressAnim->start();
}
