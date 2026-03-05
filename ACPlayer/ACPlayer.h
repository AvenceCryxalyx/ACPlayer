#pragma once

#include <QtWidgets>
#include <QMediaPlayer>
#include <QtMultimediaWidgets>
#include <QtCore>
#include <QtGui>
#include "ui_ACPlayer.h"

class ACPlayer : public QMainWindow
{
    Q_OBJECT

public:
    ACPlayer(QWidget *parent = nullptr);
    ~ACPlayer();
    void OpenWithFile(QString* fileName);
    void PlayVideo(QString* fileName);
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
    void on_QPlayer_durationChanged(qint64 position);
    void on_metaChanged();
    void initialize();
    void HandleStatusChange(QMediaPlayer::MediaStatus Status) {
        emit statusChanged(Status); // Redirect so that the main application can handle this signal too
    }

signals:
    void statusChanged(QMediaPlayer::MediaStatus);
public slots:
    void ChangedStatus(QMediaPlayer::MediaStatus);
    void MediaError(QMediaPlayer::Error);
private:
    Ui::ACPlayerClass ui;
    QMediaPlayer* Player = nullptr;
    QGraphicsView* view = nullptr;
    QGraphicsScene* scene = nullptr;
    QGraphicsVideoItem* Video = nullptr;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput* Audio = nullptr;
#endif
    qint64 mDuration;
    bool Is_Paused = true;
    bool Is_Muted = false;
    bool isInitialized = false;
    void resizeEvent(QResizeEvent* event) override;
    void updateProgressPosition(qint64 duration);

    const int millisecondsPerSecond = 1000;
    const int secondsPerMinute = 60;
    const int minutesPerHour = 60;
};

