#include "intro_window.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/favicon2.ico"));

    intro_window intro;   // intro screen
    intro.show();

    return a.exec();
}
