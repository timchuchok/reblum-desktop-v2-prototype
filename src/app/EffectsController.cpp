#include "EffectsController.h"

EffectsController::EffectsController(QObject* parent) : QObject(parent) {
    m_orange.color = QColor(200, 99, 42);
    m_green.color = QColor(58, 138, 80);
}

void EffectsController::setOrangeOpacity(float opacity) {
    m_orange.opacity = opacity;
    emit orangeSettingsChanged(m_orange);
}

void EffectsController::setOrangeThreshold(float threshold) {
    m_orange.threshold = threshold;
    emit orangeSettingsChanged(m_orange);
}

void EffectsController::setOrangeEnabled(bool enabled) {
    m_orange.enabled = enabled;
    emit orangeSettingsChanged(m_orange);
}

void EffectsController::setGreenOpacity(float opacity) {
    m_green.opacity = opacity;
    emit greenSettingsChanged(m_green);
}

void EffectsController::setGreenThreshold(float threshold) {
    m_green.threshold = threshold;
    emit greenSettingsChanged(m_green);
}

void EffectsController::setGreenEnabled(bool enabled) {
    m_green.enabled = enabled;
    emit greenSettingsChanged(m_green);
}
