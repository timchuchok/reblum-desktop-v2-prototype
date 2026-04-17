#pragma once

#include <QColor>
#include <QPixmap>
#include <QWidget>

class GradientSlider : public QWidget {
    Q_OBJECT
public:
    // fromColor = left end (value 0), toColor = right end (value 1)
    GradientSlider(QColor fromColor, QColor toColor,
                   QWidget* parent = nullptr);

    float value() const { return m_value; }
    void setValue(float value);

signals:
    void valueChanged(float value);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;

private:
    float valueFromX(int x) const;

    QColor m_fromColor;
    QColor m_toColor;
    float m_value = 0.0f;
    bool m_dragging = false;
    bool m_hovered = false;

    QPixmap m_thumbNormal;
    QPixmap m_thumbHover;
    QPixmap m_thumbDisabled;
};
