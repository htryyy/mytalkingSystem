#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTextBrowser>
#include <QFontComboBox>
#include <QComboBox>
#include <QToolButton>
#include <QLineEdit>
#include <QColor>
class toolbox1;
class Widget : public QWidget
{
    Q_OBJECT
    
public:
    Widget(const QIcon icon,int ID,QString name,toolbox1 *w,QWidget *parent=0);
    ~Widget();
    QPushButton *pushButton;
    void add_msg(QString delivername, QString msg);
private:
    QTextBrowser *textBrowser;
    QFontComboBox *fontComboBox;//改变字体
    QComboBox *comboBox;//下拉列表
    QToolButton *toolButton_1;
    QToolButton *toolButton_2;
    QToolButton *toolButton_3;
    QToolButton *toolButton_4;
    QToolButton *toolButton;


    QLineEdit *lineEdit;
    int userID;
    QString username;
    QColor color;
    toolbox1 *main_w;

    void init_widget();
private slots:
    void on_pushButton_clicked();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_toolButton_1_clicked(bool checked);
    void on_toolButton_2_clicked(bool checked);
    void on_toolButton_3_clicked(bool checked);
    void on_toolButton_4_clicked();
    void on_lineEdit_returnPressed();
    void on_toolButton_clicked();
};

#endif // WIDGET_H
