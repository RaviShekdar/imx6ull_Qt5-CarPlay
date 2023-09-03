#include "MusicListWidget.h"

MusicListWidget::MusicListWidget(QWidget* parent) : QListWidget(parent)
{

}

void MusicListWidget::refresh()
{
    clear();
    musicList.add_to_listwidget(this);
}

void MusicListWidget::set_musiclist(const MusicList& music)
{
    musicList = music;
    refresh();
}
