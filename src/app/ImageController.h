#pragma once

#include <QFutureWatcher>
#include <QObject>

#include "core/image/ImageModel.h"

class ImageController : public QObject {
    Q_OBJECT
public:
    explicit ImageController(QObject* parent = nullptr);

    void loadImage(const QString& path);
    const ImageModel& model() const { return m_model; }

signals:
    void imageLoaded(const ImageModel& model);
    void loadFailed(const QString& error);

private:
    void onLoadFinished();

    ImageModel m_model;
    QFutureWatcher<QImage>* m_watcher;
    QString m_pendingPath;
};
