#pragma once

#include <QImage>
#include <QString>

struct ImageModel {
    QImage image;
    QString path;
    QString fileName;

    bool isValid() const { return !image.isNull(); }
    QSize size() const { return image.size(); }
};
