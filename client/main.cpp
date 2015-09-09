#include <QApplication>
#include "toolbox1.h"
#include <QTextCodec>
int main(int argc, char *argv[])
{
    /*
    QTextCodec::setCodecForTr(QTextCodec::codecForName(QTextCodec::codecForLocale()->name()));    //Linux
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(QTextCodec::codecForLocale()->name()));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName(QTextCodec::codecForLocale()->name()));
    */

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");  //windows
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);


    QApplication a(argc, argv);
    toolbox1 w;
    w.resize(300,600);//设置主窗口大小
    w.show();
    
    return a.exec();
}
