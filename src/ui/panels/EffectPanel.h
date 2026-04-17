#pragma once

#include <QPropertyAnimation>
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

protected:
    bool eventFilter(QObject* obj, QEvent* e) override;

private:
    void setCollapsed(bool collapsed);

    GradientSlider* m_opacitySlider = nullptr;
    GradientSlider* m_thresholdSlider = nullptr;
    EyeButton* m_eyeBtn = nullptr;
    QWidget* m_header = nullptr;
    QWidget* m_body = nullptr;
    QPropertyAnimation* m_animation = nullptr;
    bool m_collapsed = false;
};
