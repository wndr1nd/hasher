#ifndef nox11
#include "mainwindow.h"
#include <QApplication>
#endif

#ifdef WIN32
#include <Windows.h>
#endif

#include "no_gui.h"

int main(int argc, char *argv[])
{
#ifdef WIN32
if (AttachConsole(ATTACH_PARENT_PROCESS)) {
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
}
#endif
    if (argc > 1)
    {
       if (argc == 5)
       {
       cmd_handler(argv[1], argv[2], argv[3], argv[4]);
       }
       else if (argc == 4)
       {
        cmd_handler("", argv[1], argv[2], argv[3]);
       }
       else if (argc == 3)
       {
         cmd_handler("", "", argv[1], argv[2]);
       }
       else
       {
           return 0;
       }
    }
#ifndef nox11
    else
    {
    QApplication a(argc, argv);
    MainWindow w;
    main_win(&w);
    w.show();
    return a.exec();
    }
#endif
}
