#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setFont(QFont("Microsoft YaHei",9));

    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    MainWidget w;
    w.show();

    return a.exec();
}
