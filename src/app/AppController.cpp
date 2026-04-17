#include "AppController.h"

AppController::AppController(QObject* parent) : QObject(parent) {
    m_imageController = new ImageController(this);
    m_effectsController = new EffectsController(this);
    m_undoStack = new QUndoStack(this);
}
