#include "RightPanel.h"

#include <QFrame>
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

    layout->addWidget(m_orange);
    layout->addSpacing(16);
    layout->addWidget(sep);
    layout->addSpacing(16);
    layout->addWidget(m_green);
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
}
