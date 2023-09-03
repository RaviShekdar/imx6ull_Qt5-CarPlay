#ifndef MUSICLISTWIDGET_H
#define MUSICLISTWIDGET_H
#include <QListWidget>
#include "MusicList.h"

class MusicListWidget : public QListWidget
{
private:
    //当前歌曲列表(存储的是歌曲信息)
    MusicList musicList;
    //当前展示列表项使用的图标
    QIcon icon=QIcon(":/Assets/Icon/music.png");
public:
    MusicListWidget(QWidget* parent = Q_NULLPTR);

    //刷新显示(当musicList有所变化时，需要调用)
    void refresh();
    //设置歌曲列表
    void set_musiclist(const MusicList& music);

    //设置/获取图标Icon
    void set_icon(QIcon iicon){ icon=iicon; }
    QIcon get_icon(){ return icon; }

    friend class MainWidget;
};

#endif
