#include "mainwindow.h"
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QList<QScreen *> list_screen = QGuiApplication::screens();

#if __arm__
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());
    system("echo none > /sys/class/leds/ssys-led/trigger");     //改变LED触发方式
#else
    this->resize(800, 480);
#endif

    pushButton = new QPushButton(this);
    pushButton->setMinimumSize(200, 50);
    pushButton->setGeometry((this->width() - pushButton->width()) / 2,
                            (this->height() - pushButton->height()) / 2,
                            pushButton->width(), pushButton->height());
    file.setFileName("/sys/devices/platform/leds/leds/sys-led/brightness"); //LED接口
    if(!file.exists())
        pushButton->setText("未获取到LED设备");
    getLedState();                                              //先读取LED状态
    connect(pushButton, &QPushButton::clicked, this, &MainWindow::pushButtonClicked);
}

MainWindow::~MainWindow()
{

}

void MainWindow::pushButtonClicked()
{
    setLedState();
}

bool MainWindow::getLedState()
{
    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadWrite))
        qDebug() << file.errorString();

    QTextStream in(&file);
    QString buf = in.readLine();
    qDebug() << "buf: " << buf << endl;
    file.close();

    if (buf == "1") {
        pushButton->setText("LED点亮");
        return true;
    } else {
       pushButton->setText("LED熄灭");
       return false;
    }
}

void MainWindow::setLedState()
{
    bool state = getLedState();
    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadWrite))
        qDebug() << file.errorString();

    QByteArray buf[2] = {"0", "1"};         //写入0或1，代表开关
    if (state)
        file.write(buf[0]);
    else
        file.write(buf[1]);

    file.close();
    getLedState();
}
