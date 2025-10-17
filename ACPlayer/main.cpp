#include "ACPlayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ACPlayer window;
    window.show();
    return app.exec();
}
