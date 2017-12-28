#include "mainwindow.h"
#include <QApplication>

#ifdef __APPLE__
    #include <QSurfaceFormat>
#endif

int main(int argc, char *argv[])
{
#ifdef __APPLE__
    QSurfaceFormat format;
    format.setMajorVersion( 3 );
    format.setMinorVersion( 3 );
    format.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat( format );
#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
