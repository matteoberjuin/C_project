#include <QApplication>
#include "drawarea.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DrawArea area;
    area.show();

    return app.exec();
}
