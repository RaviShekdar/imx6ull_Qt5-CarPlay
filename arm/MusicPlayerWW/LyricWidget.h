#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QFile>
#include <vector>
using namespace std;

// 表示一行歌词 (一个时间点 + 对应的歌词文本)
class LyricLine
{
public:
    qint64 time;
    QString text;
    LyricLine(qint64 time, QString text):time(time), text(text) {}
};

// 重载比较 (歌词按时间排序)
bool operator <(const LyricLine& A, const LyricLine& B);

namespace Ui {
class LyricWidget;
}

class LyricWidget : public QWidget
{
    Q_OBJECT

private:
    vector<LyricLine> lines;
public:
    explicit LyricWidget(QWidget *parent = nullptr);
    ~LyricWidget();

    // 将歌词文件的内容处理为歌词结构的QList
    bool process(QString filePath);

    // 根据时间找到对应位置的歌词
    int get_index(qint64 position);

    // 显示当前播放进度的歌词
    void show(qint64 position);

    // 根据下标获得歌词内容
    QString get_lyric_text(int index);

    // 清空歌词Label
    void clear();

private:
    Ui::LyricWidget *ui;
};

#endif
