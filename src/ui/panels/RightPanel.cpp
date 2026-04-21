#include "RightPanel.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QTimer>
#include <QVBoxLayout>

#include "EffectPanel.h"
#include "app/EffectsController.h"

RightPanel::RightPanel(EffectsController* controller, QWidget* parent)
    : QWidget(parent) {
    setObjectName("RightPanel");
    setFixedWidth(294);

    // ── Outer: vertical separator + content ──────────────
    auto* outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto* leftBorder = new QFrame(this);
    leftBorder->setFrameShape(QFrame::VLine);
    leftBorder->setObjectName("PanelSeparatorV");
    outerLayout->addWidget(leftBorder);

    auto* content = new QWidget(this);
    outerLayout->addWidget(content, 1);

    // ── Content layout ────────────────────────────────────
    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 16);
    layout->setSpacing(0);

    const auto makeSep = [content]() {
        auto* sep = new QFrame(content);
        sep->setFrameShape(QFrame::HLine);
        sep->setObjectName("PanelSeparator");
        return sep;
    };

    m_orange = new EffectPanel(EffectType::Orange, content);
    m_green  = new EffectPanel(EffectType::Green, content);

    // ── Progress bar ──────────────────────────────────────
    m_progressBar = new QProgressBar(content);
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

    layout->addSpacing(16);
    layout->addWidget(m_orange);
    layout->addWidget(makeSep());
    layout->addSpacing(16);
    layout->addWidget(m_green);
    layout->addWidget(makeSep());
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
