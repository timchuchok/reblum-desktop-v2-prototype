#pragma once

#include <QColor>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>

// Renders an SVG at the given logical size, replaces all opaque pixels
// with `color`, and tags the pixmap with the given device-pixel ratio.
inline QPixmap svgTinted(const QString& path, QSize size, QColor color,
                         qreal dpr) {
    const QSize physical = size * dpr;
    QImage img(physical, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter rp(&img);
    QSvgRenderer(path).render(&rp, QRectF({}, QSizeF(physical)));
    rp.end();

    QPainter tp(&img);
    tp.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tp.fillRect(img.rect(), color);
    tp.end();

    QPixmap px = QPixmap::fromImage(img);
    px.setDevicePixelRatio(dpr);
    return px;
}

// Builds a QIcon with @1x, @2x and @3x pixmaps so Qt picks the right
// resolution automatically on any screen.
inline QIcon svgIcon(const QString& path, QSize size, QColor color) {
    QIcon icon;
    for (qreal dpr : {1.0, 2.0, 3.0}) {
        icon.addPixmap(svgTinted(path, size, color, dpr));
    }
    return icon;
}
