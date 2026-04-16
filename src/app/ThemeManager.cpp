#include "ThemeManager.h"

#include <QApplication>
#include <QFile>

ThemeManager::ThemeManager(QObject* parent) : QObject(parent) {}

void ThemeManager::apply(Theme theme) {
    QFile f(pathFor(theme));
    if (!f.open(QFile::ReadOnly)) {
        return;
    }
    qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
    m_current = theme;
    emit themeChanged(theme);
}

QString ThemeManager::pathFor(Theme theme) {
    switch (theme) {
        case Theme::Dark:
            return ":/themes/dark.qss";
        case Theme::Light:
            return ":/themes/light.qss";
    }
    return {};
}
