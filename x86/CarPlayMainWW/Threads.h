#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QLabel>
#include <QMutex>

// 线程：日期时间显示
class Thread_DateTime : public QThread
{
    Q_OBJECT
public:
    explicit Thread_DateTime(QLabel* d, QLabel* t) : dateL(d), timeL(t), threadExist(true) {}
    void stop();
protected:
    void run();
private:
    QLabel* dateL, * timeL;
    QMutex mutex;
    bool threadExist;
};

#endif // THREAD_H
