# 基于Qt5的车载中控

基于Qt5的车载中控，最终目标是放入Linux开发板中使用触摸屏控制。

通过此项目巩固自己学习的知识，包括C/C++，Qt (QSS, SQLite)，进程通信、多线程，Linux驱动编写。

*音乐播放器的轮子主要参考：<https://github.com/NJU-TJL/Qt5-MusicPlayer>*

*代码思路也有参考：<https://blog.csdn.net/mbs520/article/details/112873809?spm=1001.2014.3001.5501>*

## 功能

### 车载主菜单 (待完善)

![20230803221835](https://image-hosting-1313474851.cos.ap-shanghai.myqcloud.com/Notes/20230803221835.png)

- 当前日期时间实时显示。
- 创建**共享内存**，实现主界面进程与播放器进程间通信。
- (arm版) 编写传感器的**驱动程序**，并采集数据。

### 音乐播放器

![20230803223119](https://image-hosting-1313474851.cos.ap-shanghai.myqcloud.com/Notes/20230803223119.png)|![20230803223203](https://image-hosting-1313474851.cos.ap-shanghai.myqcloud.com/Notes/20230803223203.png)
---|---
![20230803223237](https://image-hosting-1313474851.cos.ap-shanghai.myqcloud.com/Notes/20230803223237.png)|![20230803223417](https://image-hosting-1313474851.cos.ap-shanghai.myqcloud.com/Notes/20230803223417.png)

- 使用文件操作打开音乐文件 (这里其实不妥，本地音乐最好还是一开始就扫描好所有歌曲并加载到列表中，这样才是适合车载触摸屏的操作，后续需修改)，支持.mp3，.flac文件。
- 解析歌词文件并随播放进度显示。
- Music类解析歌曲信息 (要是改成自动扫描本地歌曲的模式，歌曲信息就解析不出来，可能是信息太大push_back到类对象需要很久时间，其实现在很多车载中控都是联网的，自带各种网络音乐应用，本地音乐就不需要解析歌曲信息这一功能了，后续再修改)。
- 歌曲列表支持整理、排序、清空，并使用Qt内置的SQLite储存目前的歌曲列表。
- 播放按钮有两个QSS样式，即正在播放和暂停状态，使用信号与槽机制实现播放状态变化按钮跟随更新，滑槽与播放进度、播放时间同样使用信号与槽机制控制。
- 右上角退出按钮按下后回到主菜单界面，播放器隐藏且能继续播放。

## 设备及环境

- 硬件：imx6ull_pro，LCD屏幕，SR04超声波测距模块
- 开发软件：Qt 5.12.8
- 操作系统：Windows、Linux
- 交叉编译工具链：arm-buildroot-linux-gnueabihf-gcc v7.5.0
- 板子上的触摸库：tslib (记得设置tslib和Qt的环境变量)

## 使用

1. QtCreator打开CarPlayMainWW和MusicPlayerWW的工程文件，可执行文件由qmake执行后生成。
2. 运行CarPlayMainWW可执行文件。

## 改进日志

- TODO
  - [ ] 读取音乐文件的方式改为本地指定路径自动读取，取消数据库增删、解析歌曲信息功能
  - [ ] 更多的应用......

- V1.1
  - [x] 使用多线程
  - [x] 添加arm版本，编写SR04超声波测距模块驱动程序，在Qt上编写传感器数据采集的线程
  - [x] Menu初始化时MusicPlayer一起启动，且MusicPlayer第一次启动时隐藏界面
