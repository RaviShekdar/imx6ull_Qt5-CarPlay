#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QDir>
#include <QPropertyAnimation>
#include <QTimer>
#include <QProcess>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QThread>
#include "Threads.h"

// 共享内存传输的一些命令
#define SWITCH_VISIBILITY 1  // 改变窗体可见性
#define VOL_DOWN 2  // 音量减小
#define VOL_UP 3  // 音量增大
#define MUSIC_PLAY 4  // 播放音乐
#define MUSIC_PAUSE 5  // 暂停音乐
#define CLOSE_APP 6  // 关闭APP

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void init_ui();
    void init_shm();
    void exec_by_num(int num);
    void shm_to_zero();

    void zoom(QPushButton* btn);  // 按钮动画

private slots:
    void on_btnQuit_clicked();

    void on_btnMusic_clicked();

private:
    Ui::MainWidget *ui;
    QTimer* timerDT, * switchAppTimer, * readTimer, * detachTimer;
    QProcess* proMusic;
    Thread_DateTime* timeShowThread;
    QSharedMemory shmMenuToMusic, shmMusicToMenu;
};

#endif // MAINWIDGET_H
