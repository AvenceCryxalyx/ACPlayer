#pragma once

#include <QtWidgets>
#include <QMediaPlayer>
#include <QtMultimediaWidgets>
#include <QtCore>
#include <QtGui>
#include <QtSerialPort>
#include <QTimer>
#include <QElapsedTimer>
#include "ui_ACPlayer.h"
#include "ui_ComportConnectionDialog.h"
#include "UARTLibrary.h"
#include "ComportConnectionDialog.h"
#include "CustomVideoItem.h"
#include "RealTimeSerialWorker.h"
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QPointer>

class ACPlayer : public QMainWindow
{
    Q_OBJECT

public:
    ACPlayer(QWidget *parent = nullptr);
    ~ACPlayer();
    void OpenWithFile(QString* fileName);
    void PlayVideo(QString* fileName);
    void OpenComportSelection();
    void connectToPort(const QString& portName);
    void CloseComportConnection();
private slots:
    void onPortOpened(bool ok, const QString& portName, const QString& error);
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
//protected:
//    void closeEvent(QCloseEvent* event) override;
private:
    QSerialPort serialPort;
    ComportConnectionDialog* comportDialog = nullptr;
    QThread* serialThread = nullptr;
    QPointer<RealTimeSerialWorker> worker;
    Ui::ACPlayerClass ui;
    QMediaPlayer* Player = nullptr;
    QGraphicsView* view = nullptr;
    QGraphicsScene* scene = nullptr;
    QGraphicsVideoItem* Video = nullptr;
    double frameRate;
    QTime CurrentVidTime;
    QTimer sendDataTimer;
    QElapsedTimer offset;
    qint64 lastVideoPosition = 0;

    qint64 mDuration;
    bool Is_Paused = true;
    bool Is_Muted = false;
    bool isInitialized = false;
    void resizeEvent(QResizeEvent* event) override;
    void updateProgressPosition(qint64 duration);
    void on_sendDataToConnectedDevice(qint64 ms);

    const int millisecondsPerSecond = 1000;
    const int secondsPerMinute = 60;
    const int minutesPerHour = 60;
};

