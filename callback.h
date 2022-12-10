#ifndef CALLBACK_H
#define CALLBACK_H
#include <QObject>

class MainWindow;
typedef void (MainWindow::*Callback)();

#endif // CALLBACK_H
