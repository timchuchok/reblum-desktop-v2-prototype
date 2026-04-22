#pragma once

#include <QAbstractButton>
#include <QPixmap>

class EyeButton : public QAbstractButton {
    Q_OBJECT
public:
    explicit EyeButton(QWidget* parent = nullptr);
    QSize sizeHint() const override { return {28, 28}; }

protected:
    void paintEvent(QPaintEvent*) override;
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;

private:
    QPixmap m_icon;
    QPixmap m_iconHover;
    bool m_hovered = false;

};
