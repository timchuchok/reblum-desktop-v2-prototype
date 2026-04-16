#include "EffectPanel.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "ui/widgets/EyeButton.h"
#include "ui/widgets/GradientSlider.h"

EffectPanel::EffectPanel(EffectType type, QWidget* parent) : QWidget(parent) {
    const bool isOrange = (type == EffectType::Orange);
    const QColor color =
        isOrange ? QColor(0xc8, 0x62, 0x2a) : QColor(0x3a, 0x8a, 0x50);
    const QString name = isOrange ? "Orange" : "Green";

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // ── Header ────────────────────────────────────────────
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(6);

    auto* nameLabel = new QLabel(name, header);
    nameLabel->setObjectName("EffectName");

    m_eyeBtn = new EyeButton(header);

    headerLayout->addWidget(m_eyeBtn);
    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();

    // ── Sliders ───────────────────────────────────────────
    auto* opacityLabel = new QLabel("Opacity", this);
    opacityLabel->setObjectName("SliderLabel");
    m_opacitySlider = new GradientSlider(color, this);

    auto* thresholdLabel = new QLabel("Threshold", this);
    thresholdLabel->setObjectName("SliderLabel");
    m_thresholdSlider = new GradientSlider(color, this);

    layout->addWidget(header);
    layout->addWidget(opacityLabel);
    layout->addWidget(m_opacitySlider);
    layout->addWidget(thresholdLabel);
    layout->addWidget(m_thresholdSlider);

    connect(m_opacitySlider, &GradientSlider::valueChanged, this,
            &EffectPanel::opacityChanged);
    connect(m_thresholdSlider, &GradientSlider::valueChanged, this,
            &EffectPanel::thresholdChanged);
    connect(m_eyeBtn, &EyeButton::toggled, this, &EffectPanel::enabledChanged);
}
