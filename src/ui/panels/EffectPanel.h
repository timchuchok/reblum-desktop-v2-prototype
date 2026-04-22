#pragma once

#include <QIcon>
#include <QVariantAnimation>
#include <QWidget>

class EyeButton;
class GradientSlider;
class QLabel;

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
    bool eventFilter(QObject* obj, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void setCollapsed(bool collapsed);
    void updateChevron();

    GradientSlider* m_opacitySlider = nullptr;
    GradientSlider* m_thresholdSlider = nullptr;
    EyeButton* m_eyeBtn = nullptr;
    QWidget* m_header = nullptr;
    QWidget* m_body = nullptr;
    QLabel* m_nameLabel = nullptr;
    QLabel* m_chevron = nullptr;
    QVariantAnimation* m_animation = nullptr;
    bool m_collapsed = false;
    bool m_headerHovered = false;

    QIcon m_iconRight;
    QIcon m_iconRightHover;
    QIcon m_iconUp;
};
