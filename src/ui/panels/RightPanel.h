#pragma once

#include <QWidget>

class EffectPanel;
class EffectsController;

class RightPanel : public QWidget {
    Q_OBJECT
public:
    explicit RightPanel(EffectsController* controller, QWidget* parent = nullptr);

private:
    EffectPanel* m_orange = nullptr;
    EffectPanel* m_green = nullptr;
};
