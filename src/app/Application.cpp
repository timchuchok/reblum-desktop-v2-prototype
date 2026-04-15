#include "Application.h"
#include "config.h"

#include <QFile>
#include <QDebug>

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
    setApplicationName(APP_NAME);
    setApplicationVersion(APP_VERSION_STR);
    setOrganizationName(APP_ORG);
}

void Application::applyTheme(const QString& qssPath)
{
    QFile f(qssPath);
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Cannot open theme file:" << qssPath;
        return;
    }
    setStyleSheet(QString::fromUtf8(f.readAll()));
}
