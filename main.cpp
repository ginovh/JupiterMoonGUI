#include <QApplication>
#include <QDialog>

#include "jupitermoon.h"
#include "astroDateTime.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    jupitermoon *dialog = new jupitermoon;
    dialog->show();

    return app.exec();
}
