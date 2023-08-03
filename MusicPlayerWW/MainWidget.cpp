#include "MainWidget.h"
#include <QUrl>
#include <QMediaPlayer>
#include <QDir>
#include <QFileDialog>
#include <QCoreApplication>
#include <QListWidget>
#include "ui_MainWidget.h"
#include "MusicListWidget.h"
#include <QDebug>
#include "MyQSS.h"
#include <QPixmap>
#include <QtSql>
#include <QMessageBox>
#include <QScroller>

// 设置全局变量
QStringList nameFile;
QString openDir;
int preNum = 0, volRecord = 50;

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    init_ui();
    init_play();
    init_sqlite();
    init_musiclist_widget();
    init_shm();

//    scan_music();
//    add_music_to_list(nameFile);
//    init_playlist();

//    QUrl baseUrl("file:///D:/Software/BaiduNetdisk/BaiduSyncdisk/Code/QtCode/0_CarPlaySys/MusicPlayer/MusicPlayerWW/Assets/Music/孙燕姿-极美.mp3");  // "file:///"不可少
//    //QUrl relativeUrl("孙燕姿-极美.mp3");
//    //qDebug() << "Url:" << baseUrl.resolved(relativeUrl) << "\n";
//    MyMusic msc(baseUrl);  // resolved：拼接绝对变量和相对变量
//    qDebug() << "Author:" << msc.author
//             << "Title:" << msc.title
//             << "AlbumTitle:" << msc.albumTitle
//             << "Duration:" << msc.duration
//             << "BitRate:" << msc.bitRate;
}

MainWidget::~MainWidget()
{
    delete ui;
}

extern QString my_format_time(qint64 msTime);  // 外部链接已定义好的函数

void MainWidget::init_ui()
{
    setFixedSize(1024, 600);
    setAttribute(Qt::WA_StyledBackground);  // 告诉窗口，应该使用设置的样式表进行渲染控件背景
    setWindowFlags(Qt::FramelessWindowHint);  // 去除标题栏
    ui->sliderVol->hide();
    ui->stackedWidget->setCurrentIndex(0);
    ui->musicListWidget->set_icon(QIcon(":/Assets/Icon/music.png"));
    // 封面图片
    QPixmap albumImg(":/Assets/Icon/non-music.png");
    ui->coverPic->setPixmap(albumImg);
    ui->coverPic->setScaledContents(true);
    // 需要图标变换的图标初始化
    ui->btnPlay->setStyleSheet(PauseStyle());
}

void MainWidget::init_play()
{
    //播放器初始化
    player= new QMediaPlayer(this);
    playlist=new QMediaPlaylist;
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playlist);
    player->setVolume(volRecord);
    // 连接播放有关的信号与槽
    connect(player, &QMediaPlayer::stateChanged, this, &MainWidget::update_btn_play);  // 播放状态改变
    connect(ui->sliderProgress, &QAbstractSlider::valueChanged, this, &MainWidget::set_position);  // 滑槽拖动带动播放跳转
    connect(player, &QMediaPlayer::positionChanged, this, &MainWidget::update_duration);  // 歌曲播放时间变化
    connect(player, &QMediaPlayer::durationChanged, this, &MainWidget::update_position);  // 滑槽位置随播放进度变化而变化
    connect(player, &QMediaPlayer::metaDataAvailableChanged, this, &MainWidget::update_info);  // 切歌更换元数据
}

void MainWidget::init_sqlite()
{
    QSqlDatabase database;
    if(QSqlDatabase::contains("qt_sql_default_connection"))  // 检查是否建立过连接了
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("Music.db");
        database.setUserName("WW");
        database.setPassword("123456");
        qDebug() << "数据库建立连接";  // 测试用
        if(!database.open())
        {
            QMessageBox::critical(this,"无法打开数据库文件：Music.db",database.lastError().databaseText());
            exit(-1);
        }
    }
    // 删除表，测试用
//    QSqlQuery querySql;
//    QString deleteTable = "drop table MusicInfo";
//    querySql.prepare(deleteTable);
//    if(querySql.exec()) qDebug() << "删除表";
    // 检查表是否存在，不存在就创建
    QSqlQuery querySql;
    querySql.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("MusicInfo"));
    if(querySql.next())
    {
        if(querySql.value(0).toInt() == 0)
        {
            QSqlQuery query;
            QString createSql = "create table MusicInfo (url varchar(200), author varchar(50), title varchar(50), duration bigint, albumTitle varchar(50), bitRate int)";
            query.prepare(createSql);
            if(query.exec()) qDebug() << "表建立";  // 测试用
        }
    }
}

void MainWidget::init_musiclist_widget()
{
    ui->musicListWidget->clear();  // 先清空
    ui->musicListWidget->setFrameShape(QListWidget::NoFrame);  // 去除边框
    ui->musicListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->musicListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 设置字体
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    ui->musicListWidget->setFont(font);
    ui->musicListWidget->setIconSize(QSize(30, 30));
    ui->musicListWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);  // 设置为像素滚动
    QScroller::grabGesture(ui->musicListWidget, QScroller::LeftMouseButtonGesture);  // 设置鼠标左键拖动
    // 从数据库恢复
    ui->musicListWidget->musicList.read_from_sql();
    ui->musicListWidget->refresh();
}

void MainWidget::shm_to_zero()
{
    // 开始写
    shmMenuToMusic.lock();
    int* sharedData = static_cast<int*>(shmMenuToMusic.data());
    *sharedData = 0;
    shmMenuToMusic.unlock();
    shmMenuToMusic.detach();
}

void MainWidget::exec_by_num(int num)
{
    switch(num)
    {
    case SWITCH_VISIBILITY:
        this->show();
        shm_to_zero();
        break;
    case VOL_UP:
        ui->sliderVol->setValue(volRecord);
        shm_to_zero();
        break;
    case VOL_DOWN:
        if(ui->sliderVol->value() > 20) ui->sliderVol->setValue(20);
        volRecord = ui->sliderVol->value();
        shm_to_zero();
        break;
    case MUSIC_PLAY:
        player->play();
        shm_to_zero();
        break;
    case MUSIC_PAUSE:
        player->pause();
        shm_to_zero();
        break;
    case CLOSE_APP:
        this->close();
        shm_to_zero();
        break;
    default:
        break;
    }
}

void MainWidget::init_shm()
{
    shmMenuToMusic.setKey(QStringLiteral("shm_for_music"));  // 音乐播放器用共享内存键值
    shmMusicToMenu.setKey(QStringLiteral("shm_for_menu"));  // 菜单用共享内存键值
    switchAppTimer = new QTimer(this);
    detachTimer = new QTimer(this);
    // 定时监测共享内存并读数据
    readTimer = new QTimer(this);
    connect(readTimer, &QTimer::timeout, [&]()
    {
        qDebug() << "等待";
        if(shmMenuToMusic.attach())
        {
            int* sharedData = static_cast<int*>(shmMenuToMusic.data());
            int currentNum = *sharedData;
            qDebug() << "MusicPlayer:" << currentNum;
            exec_by_num(currentNum);
            shmMenuToMusic.detach();
        }
    });
    readTimer->start(1000);
}

void MainWidget::update_btn_play()
{
    if(player->state() == QMediaPlayer::PlayingState)
        ui->btnPlay->setStyleSheet(PlayingStyle());
    else ui->btnPlay->setStyleSheet(PauseStyle());
}

void MainWidget::set_position(int pos)
{
    if (qAbs(player->position() - pos) > 99)
        player->setPosition(pos);
}

void MainWidget::update_duration(qint64 pos)
{
    ui->sliderProgress->setValue(static_cast<int>(pos));
    ui->lyricText->setText(my_format_time(pos) + "/" + my_format_time(player->duration()));
    if(playlist->currentIndex()>=0)ui->lyricWidget->show(pos);
}

void MainWidget::update_position(qint64 duration)
{
    ui->sliderProgress->setRange(0, static_cast<int>(duration));
    ui->sliderProgress->setEnabled(static_cast<int>(duration) > 0);
    if(!(static_cast<int>(duration) > 0))
    {
        //无音乐播放时，界面元素
        QImage image(":/Assets/Icon/non-music.png");
        ui->coverPic->setPixmap(QPixmap::fromImage(image));
        ui->musicTitleLabel->setText(u8"暂无歌曲");
        ui->musicAlbumLabel->setText(u8"");
        ui->musicAuthorLabel->setText(u8"");
        ui->lyricWidget->clear();
    }
    ui->sliderProgress->setPageStep(static_cast<int>(duration) / 10);
}

void MainWidget::update_info(bool available)
{
    if (available)
    {
        // 返回可用MP3元数据列表(调试时可以查看)
//        QStringList listInfo_debug = player->availableMetaData();
//        qDebug() << listInfo_debug;

        QString info="";
        QString author = player->metaData(QStringLiteral("Author")).toStringList().join(",");
        info.append(author);
        QString title = player->metaData(QStringLiteral("Title")).toString();
        QString albumTitle = player->metaData(QStringLiteral("AlbumTitle")).toString();
        info.append(" - "+title);
        info.append(" ["+my_format_time(player->duration())+"]");

        //封面图片（应获取"ThumbnailImage" From: https://www.zhihu.com/question/36859497）
        QImage picImage = player->metaData(QStringLiteral("ThumbnailImage")).value<QImage>();
        if(picImage.isNull()) picImage = QImage(":/Assets/Icon/non-music.png");
        ui->coverPic->setPixmap(QPixmap::fromImage(picImage));
        ui->coverPic->setScaledContents(true);
        //改变正在播放歌曲的图标
        for(int i = 0; i < playlist->mediaCount(); i++)
        {
            QListWidgetItem *p = ui->musicListWidget->item(i);
            p->setIcon(ui->musicListWidget->get_icon());
        }
        int index = playlist->currentIndex();
        QListWidgetItem *p = ui->musicListWidget->item(index);
        p->setIcon(QIcon(":/Assets/Icon/music-playing.png"));

        // 歌词界面显示的信息
        ui->musicTitleLabel->setText(title);
        ui->musicAlbumLabel->setText(u8"专辑："+albumTitle);
        ui->musicAuthorLabel->setText(u8"歌手："+author);

        // 解析歌词
        ui->lyricWidget->process(ui->musicListWidget->musicList.musicArr[index].get_lyric());
    }
}

void MainWidget::scan_music()
{
    openDir = QCoreApplication::applicationDirPath() + "/Music";
    QDir dirList(openDir);
    qDebug() << openDir;
    nameFile << "*.mp3" << "*.flac";  // 选择要过滤的文件格式
    nameFile = dirList.entryList(nameFile, QDir::Files | QDir::Readable, QDir::Name);
    qDebug() << "nameFile =" << nameFile << "nameFileSize =" << nameFile.size();
}

void MainWidget::add_music_to_list(const QStringList& fileNames)
{
    QListWidgetItem* item = NULL;
    for(int i = 0; i < fileNames.size(); i++)
    {
        ui->musicListWidget->addItem(fileNames[i]);
        item = ui->musicListWidget->item(i);
        item->setForeground(Qt::black);
        item->setSizeHint(QSize((float)600/1024*this->geometry().width(), (float)50/600*this->geometry().height()));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setIcon(QIcon(":/Assets/Icon/music.png"));
    }
}

void MainWidget::zoom(QPushButton* btn)
{
    QPropertyAnimation* animation = new QPropertyAnimation(btn,"geometry");
    //设置动画时间间隔
    animation->setDuration(400);
    //设置起始位置
    animation->setStartValue(QRect(btn->x(), btn->y()+4, btn->width(), btn->height()));
    //设置结束位置
    animation->setEndValue(QRect(btn->x(), btn->y(), btn->width(), btn->height()));
    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);
    //执行动画
    animation->start();
}

void MainWidget::on_btnVol_clicked()
{
    zoom(ui->btnVol);
    if(ui->sliderVol->isVisible()) ui->sliderVol->setVisible(false);
    else ui->sliderVol->setVisible(true);
}

void MainWidget::on_btnQuit_clicked()
{
    zoom(ui->btnQuit);

    // 向菜单的共享内存写命令
    if(shmMusicToMenu.isAttached()) shmMusicToMenu.detach();
    if(!shmMusicToMenu.create(sizeof(int), QSharedMemory::ReadWrite))
    {
        qDebug() << "Failed to create shared memory!";
        return;
    }
    // 开始写
    shmMusicToMenu.lock();
    int* sharedData = static_cast<int*>(shmMusicToMenu.data());
    *sharedData = SWITCH_VISIBILITY;
    shmMusicToMenu.unlock();

    // 延时detach内存
    detachTimer->start(1000);
    connect(detachTimer, &QTimer::timeout, [&]()
    {
        detachTimer->stop();
        shmMusicToMenu.detach();
    });

    // 延时隐藏进程
    switchAppTimer->start(500);
    connect(switchAppTimer, &QTimer::timeout, [&]()
    {
        switchAppTimer->stop();
        this->hide();
    });
    //close();
}

void MainWidget::on_btnList_clicked()
{
    zoom(ui->btnList);
    if(ui->stackedWidget->currentIndex() != 1) ui->stackedWidget->setCurrentIndex(1);
    else ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget::on_btnPlay_clicked()
{
    zoom(ui->btnPlay);
    if(player->state() == QMediaPlayer::PlayingState)
     {
        player->pause();
     }
    else if(player->state() == QMediaPlayer::PausedState){

        player->play();
    }
    // 歌单里有歌但是是停止状态时
    else if((!playlist->isEmpty()) && (player->state() == QMediaPlayer::StoppedState))
    {
        playlist->setCurrentIndex(0);
        player->play();
    }
    else if(playlist->isEmpty()) return;
}

void MainWidget::on_btnPre_clicked()
{
    zoom(ui->btnPre);
    playlist->previous();
    if(!playlist->isEmpty()) player->play();
}

void MainWidget::on_btnNext_clicked()
{
    zoom(ui->btnNext);
    playlist->next();
    if(!playlist->isEmpty()) player->play();
}

void MainWidget::init_playlist()
{
    playlist->clear();  // 先清空
    for(int i = 0; i < nameFile.size(); i++)
    {
        playlist->addMedia(QUrl(openDir + "/" + nameFile[i]));
    }
}

void MainWidget::on_sliderVol_valueChanged(int value)
{
    player->setVolume(value);
    if(value <= 0) ui->btnVol->setStyleSheet(MuteStyle());
    else ui->btnVol->setStyleSheet(VolStyle());
}

void MainWidget::on_btnPlayMode_clicked()
{
    zoom(ui->btnPlayMode);

    if(playlist->playbackMode() == QMediaPlaylist::Loop)
    {
        ui->btnPlayMode->setStyleSheet(RandomStyle());
        playlist->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(playlist->playbackMode() == QMediaPlaylist::Random)
    {
        ui->btnPlayMode->setStyleSheet(Loop41Style());
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else if(playlist->playbackMode() == QMediaPlaylist::CurrentItemInLoop)
    {
        ui->btnPlayMode->setStyleSheet(LoopStyle());
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void MainWidget::on_btnAdd_clicked()
{
    zoom(ui->btnAdd);
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);  // 设置fileDialog用来打开文件
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setWindowTitle(u8"添加本地音乐（注：自动过滤，按下\"Ctrl+A\"全选添加即可；不支持添加文件夹）");
    QStringList list;list<<"application/octet-stream";  // 字节流
    fileDialog.setMimeTypeFilters(list);
    qDebug() << openDir;
    fileDialog.setDirectory(openDir);  // 设置默认指定到文件夹
    if (fileDialog.exec() == QDialog::Accepted)
    {
       QList<QUrl> urls=fileDialog.selectedUrls();
       ui->musicListWidget->musicList.add_music(urls);
       ui->musicListWidget->refresh();
       ui->stackedWidget->setCurrentIndex(1);  // 切换到歌单列表
    }
}

void MainWidget::on_btnNeaten_clicked()
{
    zoom(ui->btnNeaten);
    ui->musicListWidget->musicList.neaten();
    ui->musicListWidget->refresh();
}

void MainWidget::on_musicListWidget_clicked(const QModelIndex &index)
{
    playlist->clear();
    ui->musicListWidget->musicList.add_to_playlist(playlist);
    int i = index.row();
    playlist->setCurrentIndex(i);
    player->play();
}

void MainWidget::on_btnSortSinger_clicked()
{
    zoom(ui->btnSortSinger);
    ui->musicListWidget->musicList.sort_by(AUTHOR);
    ui->musicListWidget->refresh();
}

void MainWidget::on_btnSortTitle_clicked()
{
    zoom(ui->btnSortTitle);
    ui->musicListWidget->musicList.sort_by(TITLE);
    ui->musicListWidget->refresh();
}

void MainWidget::on_btnSortDuration_clicked()
{
    zoom(ui->btnSortDuration);
    ui->musicListWidget->musicList.sort_by(DURATION);
    ui->musicListWidget->refresh();
}

void MainWidget::on_btnSweep_clicked()
{
    ui->musicListWidget->musicList.clear();
    ui->musicListWidget->refresh();
}

// 测试程序1：打开音乐url看看能否成功打开、有没有可用的元数据
// 学到了QUrl绝对路径和相对路径该怎么表示
//QUrl baseUrl("file:///D:/Software/BaiduNetdisk/BaiduSyncdisk/Code/QtCode/0_CarPlaySys/MusicPlayer/MusicPlayerWW/Assets/Music/");  // "file:///"不可少
//QUrl relativeUrl("林俊杰-进阶.mp3");
//Music msc(baseUrl.resolved(relativeUrl));  // resolved：拼接绝对变量和相对变量
//Music msc(QUrl::fromLocalFile("D:/Software/BaiduNetdisk/BaiduSyncdisk/Code/QtCode/0_CarPlaySys/MusicPlayer/MusicPlayerWW/Assets/Music/林俊杰-进阶.mp3"));  // 用这样表示绝对路径也可以

// 测试程序2：选择指定文件夹并筛选出指定音频文件
// QCoreApplication::applicationDirPath() 为获取应用程序运行路径
//    QFileDialog fileDialog;
//    QString openDir = QCoreApplication::applicationDirPath() + "/Music";
//    QDir dirList(openDir);
//    QStringList nameFile;
//    nameFile << "*.mp3" << "*.flac";  // 选择要过滤的文件格式
//    nameFile = dirList.entryList(nameFile, QDir::Files | QDir::Readable, QDir::Name);
//    qDebug() << "nameFiles == " << nameFile;
