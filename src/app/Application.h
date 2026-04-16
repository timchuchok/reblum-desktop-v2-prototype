#pragma once

#include <QApplication>

#include "app/ThemeManager.h"

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int& argc, char** argv);
    ~Application() override = default;

    ThemeManager* themeManager() const { return m_themeManager; }

private:
    ThemeManager* m_themeManager = nullptr;
};
