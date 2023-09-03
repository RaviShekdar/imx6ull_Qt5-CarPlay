#include "Threads.h"
#include <QDate>
#include <QTime>
#include <QDebug>
// 为传感器添加的
#include "sr04.h"
#include <sys/ioctl.h>
#include <poll.h>
#include <unistd.h>
int sr04Val;
struct pollfd fds[1];
#define CMD_TRIG 100
#define TIMEOUT_MS 5000

void Thread_DateTime::run()
{
    while(1)
    {
        mutex.lock();
        if(!threadExist) break;
        mutex.unlock();

        // 获取当前日期
        QDate currentDate = QDate::currentDate();
        dateL->setText(QStringLiteral("%1-%2-%3").arg(currentDate.year()).arg(currentDate.month()).arg(currentDate.day()));
        // 获取当前时间
        QTime currentTime = QTime::currentTime();
        timeL->setText(currentTime.toString());
        usleep(500000);  // 500ms
    }
    mutex.unlock();
}
void Thread_DateTime::stop()
{
    mutex.lock();
    threadExist = false;
    mutex.unlock();
}

void Thread_sr04::run()
{
    if(sr04_open("/dev/sr04") == -1) return;
    while(1)
    {
        mutex.lock();
        if(!threadExist) break;
        mutex.unlock();

        sr04_ioctl(CMD_TRIG, 0);
        fds[0].fd = sr04_fd;
        fds[0].events = POLLIN;
        if(sr04_poll(fds, 1, TIMEOUT_MS) == 1)
        {
            if(sr04_read(&sr04Val, 4) == 4)
            {
                if((sr04Val * 17 / 1000000) > 227) qDebug() << sr04Val * 17 / 1000000 << "cm, Far\n";
                else if((sr04Val * 17 / 1000000) > 20 && ((sr04Val * 17 / 1000000) <= 227)) qDebug() << sr04Val * 17 / 1000000 << "cm, Well\n";
                else qDebug() << sr04Val * 17 / 1000000 << "cm, Careful!\n";
            }
        }
        else qDebug() << "poll timeout/error\n";
        usleep(200000);  // 200ms
    }
}
void Thread_sr04::stop()
{
    mutex.lock();
    threadExist = false;
    mutex.unlock();
}
