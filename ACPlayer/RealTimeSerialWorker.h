#pragma once

#include <QObject>
#include <QtSerialPort>
#include <QElapsedTimer>
#include <QMutex>

class RealTimeSerialWorker : public QObject {
    Q_OBJECT
    public:
        RealTimeSerialWorker(const QString& portName);

    public slots:
        void startStreaming();
        void stopStreaming();
        // Call this slot whenever the media player updates or state changes
        void updatePlaybackState(int state, int frameRate, qint64 positionMs);

    private:
        void runStreamLoop();

        QString m_portName;
        QSerialPort* m_port;
        bool m_running;

        // Thread-safe variables to share state from main thread
        QMutex m_mutex;
        int m_state;
        int m_frameRate;
        qint64 m_basePositionMs;
        QElapsedTimer m_stateAgeTimer;
};

