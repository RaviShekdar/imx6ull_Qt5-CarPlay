#include "MainWidget.h"
#include "ui_MainWidget.h"
#include <QDebug>
#include "MyQSS.h"
#include <QTime>
#include <QDate>
#include <QString>

//QString musicAppPath = "D:/Software/BaiduNetdisk/BaiduSyncdisk/Code/QtCode/0_CarPlaySys/MusicPlayer/build-MusicPlayerWW-Desktop_Qt_5_12_8_MinGW_64_bit-Debug/debug/MusicPlayerWW.exe";  // 音乐播放器app的位置
QString musicAppPath = "./MusicPlayerWW";
int preNum = 0;

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    init_ui();
    proMusic = new QProcess();
    init_shm();
}

MainWidget::~MainWidget()
{
    qDebug() << "关闭";
    proMusic->close();
    delete ui;
}

void MainWidget::init_ui()
{
    setFixedSize(1024, 600);
    setAttribute(Qt::WA_StyledBackground);  // 告诉窗口，应该使用设置的样式表进行渲染控件背景
    setWindowFlags(Qt::FramelessWindowHint);  // 去除标题栏
    ui->dateLabel->setStyleSheet(DateLabelStyle());  // 设置日期标签的QSS
    ui->timeLabel->setStyleSheet(TimeLabelStyle());  // 设置时间标签的QSS

    // 线程：获取当前日期时间并显示
//    timerDT = new QTimer(this);
//    timerDT->start(500);
    timeShowThread = new Thread_DateTime(ui->dateLabel, ui->timeLabel);
    timeShowThread->start();
    connect(timeShowThread, &QThread::finished, timeShowThread, &QObject::deleteLater);
}

void MainWidget::shm_to_zero()
{
    // 开始写
    shmMusicToMenu.lock();
    int* sharedData = static_cast<int*>(shmMusicToMenu.data());
    *sharedData = 0;
    shmMusicToMenu.unlock();
    shmMusicToMenu.detach();
}

void MainWidget::exec_by_num(int num)
{
    switch(num)
    {
    case SWITCH_VISIBILITY:
        this->show();
        shm_to_zero();
        break;
    default:
        break;
    }
}

void MainWidget::init_shm()
{
    shmMenuToMusic.setKey(QStringLiteral("shm_for_music"));  // 音乐播放器用共享内存键值
    shmMusicToMenu.setKey(QStringLiteral("shm_for_menu"));  // 菜单用共享内存键值
    detachTimer = new QTimer(this);
    // 定时监测共享内存并读数据
    readTimer = new QTimer(this);
    connect(readTimer, &QTimer::timeout, [&]()
    {
        // qDebug() << "等待";
        if(shmMusicToMenu.attach())
        {
            int* sharedData = static_cast<int*>(shmMusicToMenu.data());
            int currentNum = *sharedData;
            qDebug() << "Menu:" << currentNum;
            exec_by_num(currentNum);
            shmMusicToMenu.detach();
        }
    });
    readTimer->start(1000);
}

void MainWidget::zoom(QPushButton* btn)
{
    QPropertyAnimation* animation = new QPropertyAnimation(btn,"geometry");
    //设置动画时间间隔
    animation->setDuration(400);
    //设置起始位置
    animation->setStartValue(QRect(btn->x(), btn->y()+4, btn->width(), btn->height()));
    //设置结束位置
    animation->setEndValue(QRect(btn->x(), btn->y(), btn->width(), btn->height()));
    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);
    //执行动画
    animation->start();
}

void MainWidget::on_btnQuit_clicked()
{
    zoom(ui->btnQuit);
//    sr04_close();
    timeShowThread->stop();
    close();
}

void MainWidget::on_btnMusic_clicked()
{
    zoom(ui->btnMusic);
    // 判断音乐播放器有没有在运行
    if(proMusic->state() == QProcess::Running)  // 如果音乐进程在
    {
        if(shmMenuToMusic.isAttached()) shmMenuToMusic.detach();  // 如果之前绑定有，解绑并创建新的
        if(!shmMenuToMusic.create(sizeof(int), QSharedMemory::ReadWrite))
        {
            qDebug() << "Failed to create shared memory!";
            return;
        }
        // 开始写
        shmMenuToMusic.lock();
        int* sharedData = static_cast<int*>(shmMenuToMusic.data());
        *sharedData = SWITCH_VISIBILITY;
        shmMenuToMusic.unlock();

        // 延时detach内存
        detachTimer->start(1000);
        connect(detachTimer, &QTimer::timeout, [&]()
        {
            detachTimer->stop();
            shmMenuToMusic.detach();
        });

        // 延时隐藏进程
        this->hide();
    }
    else if(proMusic->state() == QProcess::NotRunning)  // 如果没开过音乐进程
    {
        proMusic->close();
        proMusic->start(musicAppPath);
        // 延时隐藏进程
        this->hide();
    }
}
