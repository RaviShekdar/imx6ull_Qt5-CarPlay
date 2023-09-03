#include "LyricWidget.h"
#include "ui_LyricWidget.h"

#include <algorithm>
#include <QTextCodec>

LyricWidget::LyricWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LyricWidget)
{
    ui->setupUi(this);
    clear();  // 先清空完
}

LyricWidget::~LyricWidget()
{
    delete ui;
}

bool operator <(const LyricLine& A, const LyricLine& B)
{
    return A.time < B.time;
}

bool LyricWidget::process(QString filePath)
{
    // 打开歌词文件
    QFile lyricFile(filePath);
    lyricFile.open(QFile::ReadOnly);
    QString content(QString::fromLocal8Bit(lyricFile.readAll()));
    lyricFile.close();
    lines.clear();

    const QRegExp rx("\\[(\\d+):(\\d+(\\.\\d+)?)\\]");  // 用来查找时间标签的正则表达式

    // 步骤1
    int pos = rx.indexIn(content);
    if (pos == -1) {
        return false;
    }
    else {
        int lastPos;
        QList<int> timeLabels;
        do {
            // 步骤2
            timeLabels << (rx.cap(1).toInt() * 60 + rx.cap(2).toDouble()) * 1000;
            lastPos = pos + rx.matchedLength();
            pos = rx.indexIn(content, lastPos);
            if (pos == -1) {
                QString text = content.mid(lastPos).trimmed();
                foreach (const int& time, timeLabels)
                    lines.push_back(LyricLine(time, text));
                break;
            }
            // 步骤3
            QString text = content.mid(lastPos, pos - lastPos);
            if (!text.isEmpty()) {
                foreach (const int& time, timeLabels)
                    lines.push_back(LyricLine(time, text.trimmed()));
                timeLabels.clear();
            }
        }
        while (true);
        // 步骤4
        stable_sort(lines.begin(), lines.end());
    }
    if (lines.size()) return true;
    return false;
}

int LyricWidget::get_index(qint64 position)
{
    if(!lines.size()){
        return -1;
    }else{
        if(lines[0].time>=position){
            return 0;
        }
    }
    int i=1;
    for(i=1;i<lines.size();i++){
        if(lines[i-1].time<position && lines[i].time>=position){
            return i-1;
        }
    }
    return lines.size()-1;
}

void LyricWidget::show(qint64 position)
{
    int index=get_index(position);
    if(index==-1){
        ui->lyricP3->setText("");
        ui->lyricP2->setText("");
        ui->lyricP1->setText("");
        ui->lyricCenter->setText(u8"当前歌曲无歌词");
        ui->lyricN1->setText("");
        ui->lyricN2->setText("");
        ui->lyricN3->setText("");
    }else{
        ui->lyricP3->setText(get_lyric_text(index-3));
        ui->lyricP2->setText(get_lyric_text(index-2));
        ui->lyricP1->setText(get_lyric_text(index-1));
        ui->lyricCenter->setText(get_lyric_text(index));
        ui->lyricN1->setText(get_lyric_text(index+1));
        ui->lyricN2->setText(get_lyric_text(index+2));
        ui->lyricN3->setText(get_lyric_text(index+3));
    }
}

QString LyricWidget::get_lyric_text(int index)
{
    if(index<0 || index>=lines.size()){
        return "";
    }else{
        return lines[index].text;
    }
}

void LyricWidget::clear()
{
    ui->lyricP3->setText("");
    ui->lyricP2->setText("");
    ui->lyricP1->setText("");
    ui->lyricCenter->setText("");
    ui->lyricN1->setText("");
    ui->lyricN2->setText("");
    ui->lyricN3->setText("");
}
