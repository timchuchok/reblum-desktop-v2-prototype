#pragma once

#include <QProgressBar>
#include <QPropertyAnimation>
#include <QWidget>

class EffectPanel;
class EffectsController;

class RightPanel : public QWidget {
    Q_OBJECT
public:
    explicit RightPanel(EffectsController* controller,
                        QWidget* parent = nullptr);

private:
    void triggerProgress();

    EffectPanel* m_orange = nullptr;
    EffectPanel* m_green = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPropertyAnimation* m_progressAnim = nullptr;
};
