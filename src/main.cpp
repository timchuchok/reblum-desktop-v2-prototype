#include "app/Application.h"
#include "ui/main_window/MainWindow.h"

int main(int argc, char* argv[]) {
    Application app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
