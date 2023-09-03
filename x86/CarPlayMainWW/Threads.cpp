#include "Threads.h"
#include <QDate>
#include <QTime>
#include <QDebug>

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
