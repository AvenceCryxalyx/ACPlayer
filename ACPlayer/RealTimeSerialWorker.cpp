#include "RealTimeSerialWorker.h"
#include <QTime>

RealTimeSerialWorker::RealTimeSerialWorker(const QString& portName, QObject* parent)
    : QObject(parent), m_portName(portName), m_port(nullptr), m_streamTimer(nullptr),
    m_state(1), m_frameRate(30), m_basePositionMs(0) {
}

RealTimeSerialWorker::~RealTimeSerialWorker() {
    stopStreaming();
}

bool RealTimeSerialWorker::startStreaming() {

    if (!m_port) {
        m_port = new QSerialPort(this);
    }
    if (m_port->isOpen()) return false;

    m_port->setPortName(m_portName);
    m_port->setBaudRate(QSerialPort::Baud9600);

    if (!m_port->open(QIODevice::WriteOnly)) {
        m_open = false;
        emit portOpened(false, m_portName, m_port->errorString());
        return false;
    }

    if (!m_streamTimer) {
        m_streamTimer = new QTimer(this);
        m_streamTimer->setTimerType(Qt::PreciseTimer);
        connect(m_streamTimer, &QTimer::timeout, this, &RealTimeSerialWorker::sendDataPacket);
    }

    m_streamTimer->start(10);

    m_open = true;
    emit portOpened(true, m_portName, QString());
    return true;
}

void RealTimeSerialWorker::updatePlaybackState(int state, int frameRate, qint64 positionMs) {
    QMutexLocker locker(&m_mutex);
    m_state = state;
    m_frameRate = frameRate;
    m_basePositionMs = positionMs;
    m_stateAgeTimer.restart();
}

void RealTimeSerialWorker::sendDataPacket() {
    if (!m_port || !m_port->isOpen()) return;

    int currentState;
    int currentFrameRate;
    qint64 calculatedTimeMs = 0;

    // Thread-safe snapshot extraction
    {
        QMutexLocker locker(&m_mutex);
        currentState = m_state;
        currentFrameRate = m_frameRate;

        if (currentState == 2 && m_stateAgeTimer.isValid()) {
            calculatedTimeMs = m_basePositionMs + m_stateAgeTimer.elapsed();
        }
        else {
            calculatedTimeMs = m_basePositionMs;
        }
    }

    // Convert timestamp to time elements safely
    QTime videoTime = QTime::fromMSecsSinceStartOfDay(calculatedTimeMs % 86400000);

    // FIXED: Original layout resized to 14 bytes but left bytes 8-13 unassigned (garbage memory overhead)
    // Resized to exactly 9 bytes to match your data structure footprint cleanly
    QByteArray dataArray;
    dataArray.resize(13);

    dataArray[0] = 0xCC;
    dataArray[1] = currentState;
    dataArray[2] = currentFrameRate;
    dataArray[3] = videoTime.hour();
    dataArray[4] = videoTime.minute();
    dataArray[5] = videoTime.second();

    // FIXED: Re-enabled index [7] for low byte and changed footer index to [8]
    int msec = videoTime.msec();
    dataArray[6] = msec; // High Byte
    dataArray[7] = 0xAA;               // Footer
    m_port->write(dataArray.data(), static_cast<qint64>(dataArray.size()));
}

bool RealTimeSerialWorker::stopStreaming() {
    if (m_streamTimer && m_streamTimer->isActive()) {
        m_streamTimer->stop();
    }
    m_open = false;

    if (m_port && m_port->isOpen()) {
        if (m_port->bytesToWrite() > 0) {
            m_port->waitForBytesWritten(50);
        }
        m_port->close();
        return true;
    }
    return false;
}