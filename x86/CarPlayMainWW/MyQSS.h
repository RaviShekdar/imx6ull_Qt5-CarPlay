#ifndef MYQSS_H
#define MYQSS_H
#include <QString>

// 此文件统一存放一些QSS样式代码

// 日期标签字体
inline QString DateLabelStyle()
{
    return
            "#dateLabel"
            "{"
            "font-size:30px;"
            "font-weight:bold;"
            "background:transparent;"
            "}";
}

// 时间标签字体
inline QString TimeLabelStyle()
{
    return
            "#timeLabel"
            "{"
            "font-size:50px;"
            "font-weight:bold;"
            "background:transparent;"
            "}";
}

#endif // MYQSS_H
