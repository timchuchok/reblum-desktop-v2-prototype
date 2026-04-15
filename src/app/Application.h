#pragma once

#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int& argc, char** argv);
    ~Application() override = default;

    void applyTheme(const QString& qssPath);
};
