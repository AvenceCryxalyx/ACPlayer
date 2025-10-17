#pragma once

#include <QtWidgets>
//#include <QtMultimedia/qmediaplayer.h>
//#include <QtMultimediaWidgets/qvideowidget.h>
#include <QtMultimediaWidgets>
#include <QtMultimedia>
#include <QtCore>
#include <QtGui>
#include "ui_ACPlayer.h"

class ACPlayer : public QMainWindow
{
    Q_OBJECT

public:
    ACPlayer(QWidget *parent = nullptr);
    ~ACPlayer();

private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 duration);

    void on_actionOpenTriggered();
    void on_playToggled();
    void on_nextPressed();
    void on_prevPressed();
    void on_stopPressed();
    void on_muteToggled();
    void on_hSlider_Volume_valueChanged(int value);
    void on_hSlider_Progress_valueChanged(int value);
private:
    Ui::ACPlayerClass ui;
    QMediaPlayer* Player;
    QVideoWidget* Video;
    QAudioOutput* audio;
    qint64 mDuration;
    bool Is_Paused = true;
    bool Is_Muted = false;

    void updateDuration(qint64 duration);
};

