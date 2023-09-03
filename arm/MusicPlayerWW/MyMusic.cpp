#include "MyMusic.h"
#include <QMediaPlayer>
#include <QCoreApplication>  // 提供事件循环
#include <QtSql>
#include <QDebug>

MyMusic::MyMusic(QUrl iurl)
{
    url = iurl;
//    qDebug() << url.toString();
    init_from_url();
}

MyMusic::MyMusic(QString musicName)
{
    init_from_name(musicName);
}

QString my_format_time(qint64 msTime)
{
    qint64 seconds = msTime / 1000;  // 毫秒变为秒
    const qint64 minutes = seconds / 60;
    seconds -= minutes * 60;
    return QStringLiteral("%1:%2").arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
}

void MyMusic::init_from_url()
{
    QMediaPlayer tmpPlayer;
    tmpPlayer.setMedia(url);
    // 元数据的解析需要时间，所以这里需要循环等待 (但同时需要保持Qt事件处理机制在运行)
    while(!tmpPlayer.isMetaDataAvailable()) QCoreApplication::processEvents();
    QString list = tmpPlayer.availableMetaData().join(", ");
    if(tmpPlayer.isMetaDataAvailable())
    {
        author = tmpPlayer.metaData(QStringLiteral("Author")).toStringList().join(",");  // 作者这里返回的是StringList
        title = tmpPlayer.metaData(QStringLiteral("Title")).toString();
        albumTitle = tmpPlayer.metaData(QStringLiteral("AlbumTitle")).toString();
        duration = tmpPlayer.duration();
        bitRate = tmpPlayer.metaData(QStringLiteral("AudioBitRate")).toInt();
        // 封面图片(应获取"ThumbnailImage" From: https://www.zhihu.com/question/36859497)
        picImg = tmpPlayer.metaData(QStringLiteral("ThumbnailImage")).value<QImage>();
        if(picImg.isNull()) picImg = QImage(":/Assets/Icon/non-music.png");
    }
}

void MyMusic::init_from_name(QString musicName)
{
    QString openDir = QCoreApplication::applicationDirPath() + "/Music";
    url = openDir + "/" + musicName;
    init_from_url();
}

QString MyMusic::get_brief_info() const
{
    return author + " - " + title + " [" + my_format_time(duration) + "]";
}

QString MyMusic::get_lyric()
{
    QString filePath = url.toLocalFile();
    filePath.replace(".mp3", ".lrc");
    filePath.replace(".flac", ".lrc");
    filePath.replace(".mpga", ".lrc");

    return filePath;
}

void MyMusic::insert_sql()
{
    QSqlQuery sqlQuery;
    QString insertSql = "insert into MusicInfo values (?, ?, ?, ?, ?, ?)";
    sqlQuery.prepare(insertSql);
    sqlQuery.addBindValue(url.toString());
    sqlQuery.addBindValue(author);
    sqlQuery.addBindValue(title);
    sqlQuery.addBindValue(duration);
    sqlQuery.addBindValue(albumTitle);
    sqlQuery.addBindValue(bitRate);
    if(sqlQuery.exec()) qDebug() << "插入数据库";  // 测试用
}

bool MusicCompare::operator()(const QString& strA, const QString& strB)
{
    MyMusic A(strA);
    MyMusic B(strB);
    switch (key) {
    case TITLE:
        return A.title < B.title;
    case AUTHOR:
        return A.author < B.author;
    case DURATION:
        return A.duration < B.duration;
    case EQUALITY:  // 对比两者url是否相等
        return A.get_url() == B.get_url();
    default:
        return A.get_brief_info() < B.get_brief_info();
    }
}

bool MusicCompare::operator()(const MyMusic &A, const MyMusic &B)
{
    switch (key) {
    case TITLE:
        return A.title < B.title;
    case AUTHOR:
        return A.author < B.author;
    case DURATION:
        return A.duration < B.duration;
    case EQUALITY:  // 对比两者url是否相等
        return A.get_url() == B.get_url();
    default:
        return A.get_brief_info() < B.get_brief_info();
    }
}
