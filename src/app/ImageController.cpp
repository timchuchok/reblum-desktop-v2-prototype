#include "ImageController.h"

#include <QFileInfo>
#include <QtConcurrent>

ImageController::ImageController(QObject* parent)
    : QObject(parent), m_watcher(new QFutureWatcher<QImage>(this)) {
    connect(m_watcher, &QFutureWatcher<QImage>::finished, this,
            &ImageController::onLoadFinished);
}

void ImageController::loadImage(const QString& path) {
    m_pendingPath = path;
    // Do not cancel/wait — QtConcurrent::run cannot be cancelled.
    // onLoadFinished guards against stale results via m_pendingPath comparison.
    m_watcher->setFuture(QtConcurrent::run([path]() { return QImage(path); }));
}

void ImageController::onLoadFinished() {
    QImage img = m_watcher->result();
    if (img.isNull()) {
        emit loadFailed(tr("Failed to load image: %1").arg(m_pendingPath));
        return;
    }

    m_model.image = std::move(img);
    m_model.path = m_pendingPath;
    m_model.fileName = QFileInfo(m_pendingPath).fileName();

    emit imageLoaded(m_model);
}
