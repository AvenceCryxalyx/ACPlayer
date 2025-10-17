#include "ACPlayer.h"

ACPlayer::ACPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    Player = new QMediaPlayer();
    audio = new QAudioOutput();
    Player->setAudioOutput(audio);
    
    ui.slider_volume->setMinimum(0);
    ui.slider_volume->setMaximum(100);
    ui.slider_volume->setValue(50);
    Player->audioOutput()->setVolume(ui.slider_volume->value());

    
    ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui.pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui.pushButton_Next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui.pushButton_Prev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui.pushButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    
    

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

    ui.slider_progress->setRange(0, Player->duration() / 1000);
}

ACPlayer::~ACPlayer()
{

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

    audio->setMuted(Is_Muted);
}


void ACPlayer::on_playToggled()
{
    if (Is_Paused)
    {
        ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        Is_Paused = false;
        Player->play();
    }
    else
    {
        ui.pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        Is_Paused = true;
        Player->pause();
    }
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

void ACPlayer::on_hSlider_Progress_valueChanged(int value)
{
    Player->setPosition(value * 1000);
}

void ACPlayer::on_actionOpenTriggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Video File"), "", tr("MP4 Files (*.mp4)"));

    Video = new QVideoWidget();

    Video->setGeometry(5, 5, ui.groupBox_Video->width() - 10, ui.groupBox_Video->height() - 10);
    Video->setParent(ui.groupBox_Video);

    Player->setVideoOutput(Video);

    Player->setSource(QUrl(fileName));

    Video->setVisible(true);

    Video->show();
}

void ACPlayer::durationChanged(qint64 duration)
{
    mDuration = duration / 1000;
    ui.slider_progress->setMaximum(mDuration);
}
void ACPlayer::positionChanged(qint64 duration)
{
    if (!ui.slider_progress->isSliderDown())
    {
        ui.slider_progress->setValue(duration / 1000);
    }
    updateDuration(duration / 1000);
}

void ACPlayer::updateDuration(qint64 duration)
{
    if (duration || mDuration)
    {
        QTime CurrentTime((duration / 3600) % 60, (duration / 60) % 60, (duration * 1000) % 1000);
        QTime TotalTime((mDuration / 3600) % 60, (mDuration / 60) % 60, (mDuration * 1000) % 1000);
        QString Format = "hh:mm:ss";
        if (mDuration <= 3600) Format = "mm:ss";

        ui.label_time_elapsed->setText(CurrentTime.toString(Format));
        ui.label_time_left->setText(TotalTime.toString(Format));
    }
}