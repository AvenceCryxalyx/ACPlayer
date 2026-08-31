#pragma once
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QElapsedTimer>
#include <QGraphicsVideoItem>

class FramerateThrottler : public QAbstractVideoSurface {
    Q_OBJECT
public:
    FramerateThrottler(QGraphicsVideoItem* targetItem, QObject* parent = nullptr)
        : QAbstractVideoSurface(parent), m_targetItem(targetItem) {
        setTargetFps(30); // Set your desired frame rate here
    }

    void setTargetFps(int fps) {
        m_frameIntervalMs = 1000.0 / fps;
        m_timer.start();
    }

    // Pass format negotiations down to the actual QGraphicsVideoItem surface
    bool start(const QVideoSurfaceFormat& format) override {
        if (m_targetItem && m_targetItem->videoSurface()) {
            return m_targetItem->videoSurface()->start(format) && QAbstractVideoSurface::start(format);
        }
        return false;
    }

    void stop() override {
        if (m_targetItem && m_targetItem->videoSurface()) {
            m_targetItem->videoSurface()->stop();
        }
        QAbstractVideoSurface::stop();
    }

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override {
        if (m_targetItem && m_targetItem->videoSurface()) {
            return m_targetItem->videoSurface()->supportedPixelFormats(handleType);
        }
        return QList<QVideoFrame::PixelFormat>();
    }

    bool present(const QVideoFrame& frame) override {
        if (!frame.isValid() || !m_targetItem || !m_targetItem->videoSurface()) return false;

        // Strict frame rate throttling logic
        qint64 elapsed = m_timer.elapsed();
        if (elapsed >= m_frameIntervalMs) {
            m_timer.restart();

            // Forward the frame onto the actual QGraphicsVideoItem to paint
            return m_targetItem->videoSurface()->present(frame);
        }

        // Silently drop the frame if it comes too fast
        return true;
    }

private:
    QGraphicsVideoItem* m_targetItem;
    QElapsedTimer m_timer;
    double m_frameIntervalMs;
};
