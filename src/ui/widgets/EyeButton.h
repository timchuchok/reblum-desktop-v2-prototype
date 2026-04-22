#pragma once

#include <QAbstractButton>
#include <QIcon>

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
    QIcon m_icon;
    QIcon m_iconHover;
    bool m_hovered = false;

};
