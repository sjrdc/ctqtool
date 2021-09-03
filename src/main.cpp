#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QFile f(":qdarkstyle/style.qss");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    qApp->setStyleSheet(ts.readAll());

    CtqTool::MainWindow w;
    w.show();

    return QApplication::exec();
}
