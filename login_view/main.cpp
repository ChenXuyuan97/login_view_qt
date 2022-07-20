#include "LoginView.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginView w;
    w.show();
    return a.exec();
}
