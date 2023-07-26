#include "qform1.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QForm1 w; //declara la variable de la clase QForm1
    w.show();

    return a.exec();
}
