#include "dataassistant.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataAssistant w;
    w.show();

    return a.exec();
}
