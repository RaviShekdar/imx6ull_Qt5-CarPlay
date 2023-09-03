#ifndef MYQSS_H
#define MYQSS_H
#include <QString>

// 此文件统一存放一些QSS样式代码

// 播放中的按钮样式
inline QString PlayingStyle()
{
    return
    "#btnPlay"
    "{"
    "image:url(:/Assets/Icon/playing.png);"
    "background-repeat:no-repeat;"
    "background-color:transparent;"
    "background-position:center center;"
    "border:transparent;"
    "}";
}
// 未在播放的按钮状态
inline QString PauseStyle()
{
    return
    "#btnPlay"
    "{"
    "image:url(:/Assets/Icon/pause.png);"
    "background-repeat:no-repeat;"
    "background-color:transparent;"
    "background-position:center center;"
    "border:transparent;"
    "}";
}

// 音量按钮
inline QString VolStyle()
{
    return
    "#btnVol"
    "{"
    "image:url(:/Assets/Icon/vol.png);"
    "background-repeat:no-repeat;"
    "background-color:transparent;"
    "background-position:center center;"
    "border:transparent;"
    "}";
}
// 音量按钮静音状态
inline QString MuteStyle()
{
    return
    "#btnVol"
    "{"
    "image:url(:/Assets/Icon/mute.png);"
    "background-repeat:no-repeat;"
    "background-color:transparent;"
    "background-position:center center;"
    "border:transparent;"
    "}";
}

// 播放模式的切换
// 随机
inline QString RandomStyle()
{
    return
    "#btnPlayMode"
    "{"
    "image:url(:/Assets/Icon/random.png);"
    "background-repeat:no-repeat;"
    "background-color:transparent;"
    "background-position:center center;"
    "border:transparent;"
    "}";
}
// 列表循环
inline QString LoopStyle()
{
    return
    "#btnPlayMode"
    "{"
    "image:url(:/Assets/Icon/loop.png);"
    "background-repeat:no-repeat;"
    "background-color:transparent;"
    "background-position:center center;"
    "border:transparent;"
    "}";
}
// 单曲循环
inline QString Loop41Style()
{
    return
    "#btnPlayMode"
    "{"
    "image:url(:/Assets/Icon/loop41.png);"
    "background-repeat:no-repeat;"
    "background-color:transparent;"
    "background-position:center center;"
    "border:transparent;"
    "}";
}

inline QString ScrollBarStyle()
{
    return
            "QScrollBar:vertical"
            "{"
            "width:80px;"
            "background:rgba(248, 248, 255, 0);"
            "margin:0px,0px,0px,0px;"
            "padding-top:80px;"
            "padding-bottom:80px;"
            "}"
            // 滑块区域
            "QScrollBar::handle:vertical"
            "{"
            "background:rgba(255, 20, 147, 1);"
            "min-height:150px;"
//            "border-radius: 40px;"
            "}"
            // 上箭头
            "QScrollBar::sub-line:vertical"
            "{"
            "width:80px;"
            "height:80px;"
            "border-image:url(:/Assets/Icon/up-arrow.png);"
            "subcontrol-position:top;"
            "}"
            // 下箭头
            "QScrollBar::add-line:vertical"
            "{"
            "width:80px;"
            "height:80px;"
            "border-image:url(:/Assets/Icon/down-arrow.png);"
            "subcontrol-position:bottom;"
            "}"
            // 滑块以外的区域
            "QScrollBar::sub-page:vertical"
            "{"
            "background:rgb(220, 220, 220);"
            "}"
            "QScrollBar::add-page:vertical"
            "{"
            "background:rgb(220, 220, 220);"
            "}"
            ;
}

#endif
