#include "ACPlayer.h"

ACPlayer::ACPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    Player = new QMediaPlayer();
    Video = new QGraphicsVideoItem();
    Video->setPos(0, 0);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    view = new QGraphicsView(ui.groupBox_Video);
    scene = new QGraphicsScene(ui.groupBox_Video);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    scene = new QGraphicsScene(this);
    view = new QGraphicsView();
    //qDebug() << "View created:" << (void*)view;
    //qDebug() << "View valid:" << view->isVisible();
#endif


#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (!Audio)
    {
        Audio = new QAudioOutput();
        Player->setAudioOutput(Audio);
        ui.slider_volume->setMinimum(0);
        ui.slider_volume->setMaximum(100);
        ui.slider_volume->setValue(50);
        Player->audioOutput()->setVolume(ui.slider_volume->value());
    }
#endif

    QSize newSize = QSize(ui.groupBox_Video->width(), ui.groupBox_Video->height());
    QRectF bounds = ui.groupBox_Video->contentsRect();
    scene->setSceneRect(bounds);
    QVBoxLayout* layout = new QVBoxLayout(ui.groupBox_Video);
    layout->setContentsMargins(0, 0, 0, 0);  
    layout->addWidget(view);
    view->setScene(scene);
    view->setGeometry(0, 0, ui.groupBox_Video->width(), ui.groupBox_Video->height());
    view->updateGeometry();
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->scene()->addItem(Video);
    view->fitInView(scene->sceneRect());
    Video->setSize(view->size());
    view->show();
    Player->setVideoOutput(Video);
    
    ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui.pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui.pushButton_Next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui.pushButton_Prev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui.pushButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    
    
    connect(Player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(ChangedStatus(QMediaPlayer::MediaStatus)));
    connect(ui.actionOpen, &QAction::triggered, this, &ACPlayer::on_actionOpenTriggered);
    connect(ui.pushButton_Play, &QPushButton::clicked, this, &ACPlayer::on_playToggled);
    connect(ui.pushButton_Next, &QPushButton::clicked, this, &ACPlayer::on_nextPressed);
    connect(ui.pushButton_Prev, &QPushButton::clicked, this, &ACPlayer::on_prevPressed);
    connect(ui.pushButton_Mute, &QPushButton::clicked, this, &ACPlayer::on_muteToggled);
    connect(ui.pushButton_Stop, &QPushButton::clicked, this, &ACPlayer::on_stopPressed);
    connect(ui.slider_volume, &QAbstractSlider::valueChanged, this, &ACPlayer::on_hSlider_Volume_valueChanged);
    connect(ui.slider_progress, &QAbstractSlider::valueChanged, this, &ACPlayer::on_hSlider_Progress_valueChanged);
    connect(Player, &QMediaPlayer::durationChanged, this, &ACPlayer::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &ACPlayer::positionChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &ACPlayer::updateProgressPosition);
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(Player, & QMediaPlayer::metaDataChanged, this, &ACPlayer::on_metaChanged);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    connect(Player, &QMediaPlayer::metaDataAvailableChanged, this, &ACPlayer::on_metaChanged);
#endif
    ui.slider_progress->setRange(0, Player->duration() / 1000);
    
}

ACPlayer::~ACPlayer()
{
    delete view;
    delete Video;
    delete scene;
    delete Player;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    delete Audio;
#endif
}

void ACPlayer::on_metaChanged()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMediaMetaData metaData = Player->metaData();
    QSize resolution = metaData.value(QMediaMetaData::Resolution).toSize();
    if (resolution.height() > resolution.width()) {
        Video->setRotation(90);
    }
    else
    {
        Video->setRotation(0);
    }
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QVariant res = Player->metaData(QMediaMetaData::Resolution);
    QSize resolution;
    if (res.isValid())
    {
        resolution = res.toSize();
        if (resolution.height() > resolution.width()) {
            Video->setRotation(90);
        }
        else
        {
            Video->setRotation(0);
        }
    }
#endif

}

void ACPlayer::initialize()
{
    QStringList pars = QApplication::arguments();
    if (pars.count() > 1)
    {
        QString filePath = QFileInfo(pars[1]).absoluteFilePath();
        PlayVideo(&filePath);
    }
    isInitialized = true;
}

void ACPlayer::on_muteToggled()
{
    if (Is_Muted)
    {
        Is_Muted = false;
        ui.pushButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
    else
    {
        Is_Muted = true;
        ui.pushButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Audio->setMuted(Is_Muted);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Player->setMuted(Is_Muted);
#else

#endif

}


void ACPlayer::on_playToggled()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (Player->mediaStatus() == 0)
    {
        return;
    }
    else if(Player->mediaStatus() == QMediaPlayer::MediaStatus::EndOfMedia)
    {
        Player->setPosition(0);
        Player->play();
    }
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (Player->mediaStatus() == QMediaPlayer::NoMedia || Player->mediaStatus() == QMediaPlayer::InvalidMedia)
    {
        return;
    }
    else if (Player->mediaStatus() == QMediaPlayer::MediaStatus::EndOfMedia)
    {
        Player->setPosition(0);
        Player->play();
    }
#endif
    if (Is_Paused)
    {
        ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        ui.pushButton_Play->setText("Pause");
        Player->play();
    }
    else if(!Is_Paused)
    {
        ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        ui.pushButton_Play->setText("Play");
        Player->pause();
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Is_Paused = !Player->isPlaying();
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Is_Paused = Player->state() != QMediaPlayer::PlayingState;
#endif
}

void ACPlayer::on_nextPressed()
{

}

void ACPlayer::on_prevPressed()
{

}

void ACPlayer::on_stopPressed()
{
    Player->stop();
    ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui.pushButton_Play->setText("Play");
    Is_Paused = true;
}

void ACPlayer::on_hSlider_Volume_valueChanged(int value)
{
    float newValue = (float)value / 100;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Audio->setVolume(newValue);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Player->setVolume(newValue);
#endif
}

void ACPlayer::on_QPlayer_durationChanged(qint64 position)
{

}

void ACPlayer::on_hSlider_Progress_valueChanged(int value)
{
    Player->setPosition(value * 1000);
}

void ACPlayer::OpenWithFile(QString* fileName)
{
    //QMessageBox::warning(this, "File Path", *fileName, QMessageBox::StandardButton::Close);
    //PlayVideo(fileName);
}

void ACPlayer::PlayVideo(QString* fileName)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (Player->isPlaying())
    {
        Player->stop();
    }
    Player->setSource(QUrl(*fileName));
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (Player->state() == QMediaPlayer::PlayingState)
    {
        Player->stop();
    }
    Player->setMedia(QUrl(*fileName));
#endif
    QSize newSize = QSize(ui.groupBox_Video->width(), ui.groupBox_Video->height());
    QRectF bounds = ui.groupBox_Video->contentsRect();
    scene->setSceneRect(bounds);
    Video->setAspectRatioMode(Qt::KeepAspectRatio);
    view->fitInView(scene->sceneRect());
    view->setMinimumSize(newSize);
    on_playToggled();
}

void ACPlayer::on_actionOpenTriggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Video File"), "", tr("MP4 Files (*.mp4 *.avi *.mkv *.mov *.flv *.wmv *.webm *.mov)"));

    PlayVideo(&fileName);
}

void ACPlayer::durationChanged(qint64 duration)
{
    mDuration = duration / 1000;
    ui.slider_progress->setMaximum(mDuration);

    QString Format = "hh:mm:ss";
    if (mDuration <= 3600) Format = "mm:ss";

    int hours = (mDuration / 3600);
    int minutes = (mDuration / 60) % 60;
    int seconds = (mDuration % 1000) % 60;

    QTime TotalTime(hours, minutes, seconds);
    ui.label_time_left->setText(TotalTime.toString(Format));
}

void ACPlayer::positionChanged(qint64 duration)
{
    if (!ui.slider_progress->isSliderDown())
    {
        ui.slider_progress->blockSignals(true);
        ui.slider_progress->setValue(duration/1000);
        ui.slider_progress->blockSignals(false);
    }
}

void ACPlayer::resizeEvent(QResizeEvent* event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QSize newSize = QSize(ui.groupBox_Video->width(), ui.groupBox_Video->height());
    QRectF bounds = ui.groupBox_Video->contentsRect();
    scene->setSceneRect(bounds);
    view->resize(newSize);
    Video->setSize(newSize);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (!scene || !view || !Video || !isInitialized) {
        qDebug() << "Scene or view is null!";
        return;
    }
    QSize newSize = QSize(ui.groupBox_Video->width(), ui.groupBox_Video->height());
    QRectF bounds = ui.groupBox_Video->contentsRect();
    scene->setSceneRect(bounds);
    view->resize(newSize);
    view->fitInView(scene->sceneRect());
    Video->setSize(newSize);
#endif
}

void ACPlayer::updateProgressPosition(qint64 duration)
{

    if (duration || mDuration)
    {
        int formatDividend1 = millisecondsPerSecond;
        int formatDividend2 = millisecondsPerSecond * secondsPerMinute;
        int formatDividend3 = millisecondsPerSecond * secondsPerMinute * minutesPerHour;

        QTime CurrentTime((duration/ formatDividend3), (duration / formatDividend2) % 60,(duration / formatDividend1) % 60);
        
        QString Format = "hh:mm:ss";
        if (mDuration <= 3600)
        {
            Format = "mm:ss.zzz";
        }
        ui.label_time_elapsed->setText(CurrentTime.toString(Format));
    }
}

void ACPlayer::ChangedStatus(QMediaPlayer::MediaStatus status)
{
    //QString message = QString::fromStdString(std::to_string(status));
    //QMessageBox::warning(this, "Status", message, QMessageBox::StandardButton::Close);
}

void ACPlayer::MediaError(QMediaPlayer::Error error)
{

}