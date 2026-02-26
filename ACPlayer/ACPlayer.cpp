#include "ACPlayer.h"

ACPlayer::ACPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    Player = new QMediaPlayer();
    if (!audio)
    {
        audio = new QAudioOutput();
    }
    
    if (!Video)
    {
        Video = new QVideoWidget();
        Video->setGeometry(5, 5, ui.groupBox_Video->width() - 10, ui.groupBox_Video->height() - 10);
        Video->setParent(ui.groupBox_Video);
    }
    Player->setAudioOutput(audio);
    Player->setVideoOutput(Video);

    ui.slider_volume->setMinimum(0);
    ui.slider_volume->setMaximum(100);
    ui.slider_volume->setValue(50);
    Player->audioOutput()->setVolume(ui.slider_volume->value());

    
    ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui.pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui.pushButton_Next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui.pushButton_Prev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui.pushButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    
    
    connect(Player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(ChangedStatus(QMediaPlayer::MediaStatus)));
    connect(ui.actionOpen, &QAction::triggered, this, &ACPlayer::on_actionOpenTriggered);
    connect(ui.pushButton_Play, &QPushButton::clicked, this, &ACPlayer::on_playToggled);
    connect(ui.pushButton_Next, &QPushButton::clicked, this, &ACPlayer::on_nextPressed);
    connect(ui.pushButton_Next, &QPushButton::clicked, this, &ACPlayer::on_nextPressed);
    connect(ui.pushButton_Prev, &QPushButton::clicked, this, &ACPlayer::on_prevPressed);
    connect(ui.pushButton_Mute, &QPushButton::clicked, this, &ACPlayer::on_muteToggled);
    connect(ui.slider_volume, &QAbstractSlider::valueChanged, this, &ACPlayer::on_hSlider_Volume_valueChanged);
    connect(ui.slider_progress, &QAbstractSlider::valueChanged, this, &ACPlayer::on_hSlider_Progress_valueChanged);
    connect(Player, &QMediaPlayer::durationChanged, this, &ACPlayer::durationChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &ACPlayer::positionChanged);
    connect(Player, &QMediaPlayer::positionChanged, this, &ACPlayer::updateProgressPosition);
    ui.slider_progress->setRange(0, Player->duration() / 1000);
}

ACPlayer::~ACPlayer()
{

}

//void ACPlayer::initialize()
//{
//    QStringList pars = QApplication::arguments();
//    if (pars.count() > 1)
//    {
//        QString filePath = QFileInfo(pars[1]).absoluteFilePath();
//        PlayVideo(&filePath);
//    }
//}

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

    audio->setMuted(Is_Muted);
}


void ACPlayer::on_playToggled()
{
    if (Player->mediaStatus() == 0)
    {
        return;
    }
    else if(Player->mediaStatus() == QMediaPlayer::MediaStatus::EndOfMedia)
    {
        Player->setPosition(0);
        Player->play();
    }

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

    Is_Paused = !Player->isPlaying();
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
}

void ACPlayer::on_hSlider_Volume_valueChanged(int value)
{
    float newValue = (float)value / 100;
    audio->setVolume(newValue);
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
    if(Player->isPlaying())
    {
        Player->stop();
    }
    Player->setSource(QUrl(*fileName));
    Video->setVisible(true);

    on_playToggled();
}

void ACPlayer::on_actionOpenTriggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Video File"), "", tr("MP4 Files (*.mp4 *.avi *.mkv *.mov *.flv *.wmv *.webm)"));

    PlayVideo(&fileName);
}

void ACPlayer::durationChanged(qint64 duration)
{
    mDuration = duration / 1000;
    ui.slider_progress->setMaximum(mDuration);

    QString Format = "hh:mm:ss";
    if (mDuration <= 3600) Format = "mm:ss";

    QTime TotalTime((mDuration / 3600) % 60, (mDuration / 60) % 60, (mDuration * 1000) % 1000);
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

void ACPlayer::updateProgressPosition(qint64 duration)
{

    if (duration || mDuration)
    {
        int formatDividend1 = millisecondsPerSecond;
        int formatDividend2 = millisecondsPerSecond * secondsPerMinute;
        int formatDividend3 = millisecondsPerSecond * secondsPerMinute * minutesPerHour;

        QTime CurrentTime((duration/ formatDividend3), (duration / formatDividend2) % 60,(duration / formatDividend1) % 60);
        
        QString Format = "hh:mm:ss";
        if (mDuration <= 3600) Format = "mm:ss";
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