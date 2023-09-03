#ifndef MYMUSIC_H
#define MYMUSIC_H
#include <QString>
#include <QUrl>
#include <QImage>

// 歌曲类；用于描述单个歌曲
class MyMusic
{
private:
    QUrl url;  // 歌曲地址
    QString author;  // 作者
    QString title;  // 歌名
    QString albumTitle;  // 唱片集
    qint64 duration;  // 时长
    int bitRate;  // 比特率
    QImage picImg;  // 专辑封面

    void init_from_url();  // 私有方法：根据Url获取歌曲各信息
    void init_from_name(QString musicName);  // 私有方法：根据歌曲名获取歌曲各信息

public:
    MyMusic() {}
    MyMusic(QUrl iurl);
    MyMusic(QString musicName);

    QUrl get_url() const {return url;}  // 返回歌曲地址
    QString get_brief_info() const;  // 以一行返回歌曲的信息
    QString get_lyric();  // 根据文件名来获取歌词路径
    void insert_sql();  // 添加至数据库

    friend class MusicCompare;
    friend class MusicList;
    friend class MainWidget;
};

// 排序的可选属性
enum COMPARE{DEFAULT, TITLE, AUTHOR, DURATION, EQUALITY};

// 排序对比类：用于
class MusicCompare
{
    COMPARE key;
public:
    MusicCompare(){key=DEFAULT;}
    MusicCompare(COMPARE ikey){key=ikey;}
    bool operator()(const MyMusic& A, const MyMusic& B);
    bool operator()(const QString& strA, const QString& strB);
};

#endif // MYMUSIC_H
