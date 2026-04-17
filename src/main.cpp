#include "app/Application.h"
#include "ui/main_window/MainWindow.h"

int main(int argc, char* argv[]) {
    Application app(argc, argv);

    MainWindow window;
    app.themeManager()->apply(app.themeManager()->current());
    window.show();

    return app.exec();
}
