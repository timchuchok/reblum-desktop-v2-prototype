#pragma once

#include <QObject>
#include <QUndoStack>

#include "app/EffectsController.h"
#include "app/ImageController.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);

    ImageController* imageController() const { return m_imageController; }
    EffectsController* effectsController() const { return m_effectsController; }
    QUndoStack* undoStack() const { return m_undoStack; }

private:
    ImageController* m_imageController = nullptr;
    EffectsController* m_effectsController = nullptr;
    QUndoStack* m_undoStack = nullptr;
};
