#include "RealTimeSerialWorker.h"
#include <QThread>
#include <QCoreApplication>
#include <QTime>
#include <QDebug>

RealTimeSerialWorker::RealTimeSerialWorker(const QString& portName)
    : m_portName(portName), m_port(nullptr), m_running(false), m_state(1), m_frameRate(30), m_basePositionMs(0) {
}

void RealTimeSerialWorker::startStreaming() {
    m_port = new QSerialPort();
    m_port->setPortName(m_portName);
    m_port->setBaudRate(QSerialPort::Baud115200); // 115200 minimum for 1ms streaming!

    if (!m_port->open(QIODevice::WriteOnly)) {
        //qWarning() << "Failed to open serial port:" << m_port->errorString();
        return;
    }

    m_running = true;
    runStreamLoop();
}

void RealTimeSerialWorker::updatePlaybackState(int state, int frameRate, qint64 positionMs) {
    QMutexLocker locker(&m_mutex);
    m_state = state;
    m_frameRate = frameRate;
    m_basePositionMs = positionMs;
    m_stateAgeTimer.restart(); // Track exactly when this update arrived
}

void RealTimeSerialWorker::runStreamLoop() {
    QElapsedTimer loopTimer;
    loopTimer.start();

    qint64 nextTargetTimeMs = 1;

    while (m_running) {
        qint64 currentTimeMs = loopTimer.elapsed();

        while (currentTimeMs >= nextTargetTimeMs) {

            // Fetch current state details safely
            int currentState;
            int currentFrameRate;
            qint64 calculatedTimeMs = 0;

            {
                QMutexLocker locker(&m_mutex);
                currentState = m_state;
                currentFrameRate = m_frameRate;

                // If playing, interpolate the milliseconds between media player updates
                if (currentState == 2 && m_stateAgeTimer.isValid()) {
                    calculatedTimeMs = m_basePositionMs + m_stateAgeTimer.elapsed();
                }
                else {
                    calculatedTimeMs = m_basePositionMs;
                }
            }

            // Convert total millisecond timestamp into H:M:S:MS
            QTime videoTime = QTime::fromMSecsSinceStartOfDay(calculatedTimeMs % 86400000);

            // Construct payload packet (Expanded to 14 bytes to fit full milliseconds)
            QByteArray dataArray;
            dataArray.resize(14);

            dataArray[0] = static_cast<char>(0xCC);
            dataArray[1] = static_cast<char>(currentState);
            dataArray[2] = static_cast<char>(currentFrameRate);
            dataArray[3] = static_cast<char>(videoTime.hour());
            dataArray[4] = static_cast<char>(videoTime.minute());
            dataArray[5] = static_cast<char>(videoTime.second());

            // FIX: Split 16-bit Millisecond integer into two bytes (High Byte / Low Byte)
            int msec = videoTime.msec();
            dataArray[6] = static_cast<char>((msec >> 8) & 0xFF); // High Byte
            dataArray[7] = static_cast<char>(msec & 0xFF);        // Low Byte

            dataArray[8] = static_cast<char>(0xAA);

            // Write straight to hardware buffer
            if (m_port && m_port->isOpen()) {
                m_port->write(dataArray);
                m_port->flush();
            }

            nextTargetTimeMs++;
        }

        QCoreApplication::processEvents();
        QThread::usleep(100);
    }

    m_port->close();
    m_port->deleteLater();
}

void RealTimeSerialWorker::stopStreaming() {
    m_running = false;
}