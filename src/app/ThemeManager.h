#pragma once

#include <QObject>

class ThemeManager : public QObject {
    Q_OBJECT
public:
    enum class Theme { Dark, Light };

    explicit ThemeManager(QObject* parent = nullptr);

    void apply(Theme theme);
    Theme current() const { return m_current; }

signals:
    void themeChanged(Theme theme);

private:
    static QString pathFor(Theme theme);

    Theme m_current = Theme::Dark;
};
