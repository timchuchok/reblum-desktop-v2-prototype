#include "EyeButton.h"

#include <QImage>
#include <QPainter>
#include <QSvgRenderer>

static QPixmap renderSvgTinted(const QString& path, QSize size, QColor tint) {
    QSvgRenderer renderer(path);
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter rp(&image);
    renderer.render(&rp);
    rp.end();

    // Replace icon color with tint, preserving alpha
    QPainter tp(&image);
    tp.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tp.fillRect(image.rect(), tint);
    tp.end();

    return QPixmap::fromImage(image);
}

EyeButton::EyeButton(QWidget* parent) : QAbstractButton(parent) {
    setCursor(Qt::PointingHandCursor);
    setFixedSize(20, 20);

    m_icon = renderSvgTinted(":/icons/eye-show.svg", QSize(20, 20),
                              QColor(210, 210, 210));
}

void EyeButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.drawPixmap(0, 0, m_icon);
}
