#ifndef MUSICLIST_H
#define MUSICLIST_H
#include "MyMusic.h"
#include <QMediaPlaylist>
#include <QListWidget>
#include <vector>
using namespace std;

class MusicListWidget;

class MusicList
{
private:
    // 所存储的歌曲
    vector<MyMusic> musicArr;
public:
    MusicList() {}
    MusicList(const QList<QUrl>& urls);

    // 从url添加歌曲
    void add_music(const QList<QUrl>& urls);

    // 添加一首歌曲
    void add_music(const MyMusic& iMusic);

    //获取指定位置的歌曲
    MyMusic get_music(int pos);

    // 将本歌单加入播放列表
    void add_to_playlist(QMediaPlaylist* playlist);

    // 歌单可视化
    void add_to_listwidget(MusicListWidget* listWidget);

    // 将本列表中的歌曲排序
    void sort_by(COMPARE key);

    // 整理歌单，去掉重复的
    void neaten();

    // 清空本歌单
    void clear();

    // 数据库移除全部歌曲
    void remove_sql_all();

    // 数据库加入全部歌曲
    void insert_sql_all();

    // 从数据库中恢复歌单
    void read_from_sql();

    friend class MainWidget;
};

#endif
