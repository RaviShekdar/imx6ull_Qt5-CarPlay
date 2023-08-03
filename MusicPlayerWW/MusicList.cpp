#include "MusicList.h"
#include <algorithm>
#include <QCoreApplication>
#include <QProgressDialog>
#include <QtSql>
#include "MusicListWidget.h"
#include <QDebug>

MusicList::MusicList(const QList<QUrl> &urls)
{
    add_music(urls);
}

void MusicList::add_music(const QList<QUrl> &urls)
{
    //实测这里耗时较长，所以添加一个进度显示对话框
    QProgressDialog proDialog(u8"添加进度",u8"取消",0,urls.size());
    proDialog.setMinimumSize(350,150);
    proDialog.setWindowModality(Qt::WindowModal);
    proDialog.setWindowTitle("添加中...请稍后");
    proDialog.show();
    int x=0;
    foreach (QUrl i, urls) {
        x++;
        proDialog.setValue(x);
        //过滤Url的类型
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(i.toLocalFile());
        if(mime.name()!="audio/mpeg"&&mime.name()!="audio/flac")
        {
            continue;
        }
        //剩下的符合类型
        musicArr.push_back(MyMusic(i));
        musicArr[musicArr.size() - 1].insert_sql();  // 放入数据库
        if(proDialog.wasCanceled()) break;
    }
}

void MusicList::add_music(const MyMusic &iMusic)
{
    musicArr.push_back(iMusic);
    musicArr[musicArr.size() - 1].insert_sql();  // 放入数据库
}

MyMusic MusicList::get_music(int pos)
{
    return musicArr[pos];
}

void MusicList::add_to_playlist(QMediaPlaylist* playlist)
{
    for(auto i=musicArr.begin();i!=musicArr.end();i++)
    {
        playlist->addMedia(i->get_url());
    }
}

void MusicList::add_to_listwidget(MusicListWidget* listWidget)
{
    foreach(const MyMusic &i, musicArr)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(listWidget->get_icon());
        item->setText(i.get_brief_info());
        item->setForeground(Qt::black);
        item->setSizeHint(QSize(1000, 70));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        listWidget->addItem(item);
    }
}

void MusicList::sort_by(COMPARE key)
{
    sort(musicArr.begin(), musicArr.end(), MusicCompare(key));
    remove_sql_all();
    insert_sql_all();
}

void MusicList::neaten()
{
    sort(musicArr.begin(), musicArr.end(), MusicCompare(DEFAULT));
    musicArr.erase(unique(musicArr.begin(), musicArr.end(), MusicCompare(EQUALITY)), musicArr.end());
    remove_sql_all();
    insert_sql_all();
}

void MusicList::clear()
{
    musicArr.clear();
    remove_sql_all();
}

void MusicList::remove_sql_all()
{
    QSqlQuery sqlQuery;
    QString deleteSql = "delete from MusicInfo";
    sqlQuery.prepare(deleteSql);
    sqlQuery.exec();
}

void MusicList::insert_sql_all()
{
    for(auto i = musicArr.begin(); i < musicArr.end(); ++i)
        i->insert_sql();
}

void MusicList::read_from_sql()
{
    QSqlQuery sqlQuery;
    QString selectSql = "select url, author, title, duration, albumTitle, bitRate from MusicInfo";
    sqlQuery.prepare(selectSql);
    if(sqlQuery.exec())
    {
        while(sqlQuery.next())
        {
            MyMusic tempMusic;
            tempMusic.url = QUrl(sqlQuery.value(0).toString());
            tempMusic.author = sqlQuery.value(1).toString();
            tempMusic.title = sqlQuery.value(2).toString();
            tempMusic.duration = sqlQuery.value(3).toLongLong();
            tempMusic.albumTitle = sqlQuery.value(4).toString();
            tempMusic.bitRate = sqlQuery.value(5).toInt();
            musicArr.push_back(tempMusic);
            qDebug() << tempMusic.author << "-" << tempMusic.title << "-" << tempMusic.duration;  // 测试用
        }
    }
}
