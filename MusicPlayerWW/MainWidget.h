#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QListWidgetItem>
#include <QFileInfoList>
#include <QTimer>
#include <QSharedMemory>
#include "MyMusic.h"
#include "MusicList.h"

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
    // 初始化
    void init_ui();  // 初始化UI和按钮图标

    void init_play();  // 初始化播放器
    void init_sqlite();  // 初始化数据库sqlite
    void init_musiclist_widget();  // 初始化MusicListWidget的格式

    void init_shm();
    void exec_by_num(int num);  // 根据共享内存传来的命令，执行相应的任务
    void shm_to_zero();  // 给共享内存置0

    void update_btn_play();  // 切换播放按钮的QSS
    void set_position(int pos);
    void update_position(qint64 duration);
    void update_duration(qint64 pos);
    void update_info(bool available);

    // 自动扫描歌曲并添加到列表所用到的3种方法，暂时不用
    void init_playlist();
    void scan_music();
    void add_music_to_list(const QStringList& fileNames);

    void zoom(QPushButton* btn);  // 按钮动画

private slots:
    void on_btnVol_clicked();

    void on_btnQuit_clicked();

    void on_btnList_clicked();

    void on_btnPlay_clicked();

    void on_btnPre_clicked();

    void on_btnNext_clicked();

    void on_sliderVol_valueChanged(int value);

    void on_btnPlayMode_clicked();

    void on_btnAdd_clicked();

    void on_btnNeaten_clicked();

    void on_musicListWidget_clicked(const QModelIndex &index);

    void on_btnSortSinger_clicked();

    void on_btnSortTitle_clicked();

    void on_btnSortDuration_clicked();

    void on_btnSweep_clicked();

private:
    Ui::MainWidget *ui;

    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    QTimer* switchAppTimer, * readTimer, * detachTimer;
    QSharedMemory shmMenuToMusic, shmMusicToMenu;
};
#endif
