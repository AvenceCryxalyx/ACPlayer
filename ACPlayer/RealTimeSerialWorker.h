#pragma once
#include <atomic>
#include <QObject>
#include <QtSerialPort>
#include <QElapsedTimer>
#include <QMutex>
#include <QTimer>

class RealTimeSerialWorker : public QObject {
    Q_OBJECT
public:
    explicit RealTimeSerialWorker(const QString& portName, QObject* parent = nullptr);
    ~RealTimeSerialWorker();

    bool IsOpen() const { return m_open.load(); }
    QString PortName() const { return m_portName; }
signals:
    void portOpened(bool ok, const QString& portName, const QString& error);
public slots:
    bool startStreaming();
    bool stopStreaming();
    void updatePlaybackState(int state, int frameRate, qint64 positionMs);
    
private slots:
    void sendDataPacket(); // Triggered every 1ms by the timer

private:
    const QString m_portName;
    std::atomic<bool> m_open{ false };
    QSerialPort* m_port;
    QTimer* m_streamTimer;

    // Mutex guards all shared data modified by updatePlaybackState
    QMutex m_mutex;
    int m_state;
    int m_frameRate;
    qint64 m_basePositionMs;
    QElapsedTimer m_stateAgeTimer;
};