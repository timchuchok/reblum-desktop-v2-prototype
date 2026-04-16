#include "Application.h"

#include "config.h"

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {
    setApplicationName(APP_NAME);
    setApplicationVersion(APP_VERSION_STR);
    setOrganizationName(APP_ORG);

    m_themeManager = new ThemeManager(this);
    m_themeManager->apply(ThemeManager::Theme::Dark);
}
