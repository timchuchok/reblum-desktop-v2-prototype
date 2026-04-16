#pragma once

#include <QWidget>

class EyeButton;
class GradientSlider;

enum class EffectType { Orange, Green };

class EffectPanel : public QWidget {
    Q_OBJECT
public:
    explicit EffectPanel(EffectType type, QWidget* parent = nullptr);

signals:
    void opacityChanged(float value);
    void thresholdChanged(float value);
    void enabledChanged(bool enabled);

private:
    GradientSlider* m_opacitySlider = nullptr;
    GradientSlider* m_thresholdSlider = nullptr;
    EyeButton* m_eyeBtn = nullptr;
};
