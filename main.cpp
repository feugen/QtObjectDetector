#include "qtobjectdetector.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtObjectDetector w;
    w.show();
    return a.exec();
}
