#pragma once

#include <QAbstractButton>
#include <QPixmap>

class EyeButton : public QAbstractButton {
    Q_OBJECT
public:
    explicit EyeButton(QWidget* parent = nullptr);
    QSize sizeHint() const override { return {20, 20}; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QPixmap m_icon;
};
