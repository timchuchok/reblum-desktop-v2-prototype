#pragma once

#include <QColor>
#include <QWidget>

class GradientSlider : public QWidget {
    Q_OBJECT
public:
    explicit GradientSlider(QColor color, QWidget* parent = nullptr);

    float value() const { return m_value; }
    void setValue(float value);

signals:
    void valueChanged(float value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    float valueFromX(int x) const;

    QColor m_color;
    float m_value = 0.0f;
    bool m_dragging = false;
};
