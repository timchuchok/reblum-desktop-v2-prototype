#pragma once

#include <QObject>

#include "core/effects/EffectSettings.h"

class EffectsController : public QObject {
    Q_OBJECT
public:
    explicit EffectsController(QObject* parent = nullptr);

    const EffectSettings& orangeSettings() const { return m_orange; }
    const EffectSettings& greenSettings() const { return m_green; }

public slots:
    void setOrangeOpacity(float opacity);
    void setOrangeThreshold(float threshold);
    void setOrangeEnabled(bool enabled);
    void setGreenOpacity(float opacity);
    void setGreenThreshold(float threshold);
    void setGreenEnabled(bool enabled);

signals:
    void orangeSettingsChanged(const EffectSettings& settings);
    void greenSettingsChanged(const EffectSettings& settings);

private:
    EffectSettings m_orange;
    EffectSettings m_green;
};
