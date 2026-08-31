#pragma once
#include <QMediaPlayer>
#include <QtMultimediaWidgets>
#include "Events.h"

class CustomVideoItem : public QGraphicsVideoItem
{
	Q_OBJECT
public: 
    CustomVideoItem(QGraphicsItem* parent = nullptr, int frameInterceptorInterval = 25) : QGraphicsVideoItem(parent)
    { 
        m_surface = new FrameInterceptorSurface(this, frameInterceptorInterval); 
        
    }
    QAbstractVideoSurface* videoSurface() const;
private:
    class FrameInterceptorSurface : public QAbstractVideoSurface {
        CustomVideoItem* m_item;
        int frameCount = 0;
        int targetFrames = 20;

    public:
        FrameInterceptorSurface(CustomVideoItem* item, int targetFrames = 25) : m_item(item) 
        {
            this->targetFrames = targetFrames;
            EvtFrameIntervalTriggered = new Events();
        }
        ~FrameInterceptorSurface() 
        {
            if(EvtFrameIntervalTriggered != nullptr)
                delete EvtFrameIntervalTriggered;
        }
        Events* EvtFrameIntervalTriggered;
        QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override {
            return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_ARGB32 << QVideoFrame::Format_YUV420P;
        }

        bool present(const QVideoFrame& frame) override {
            frameCount++;
            if (frameCount >= targetFrames) {
                EvtFrameIntervalTriggered->Invoke();
                frameCount = 0;
            }
            return m_item->videoSurface()->present(frame);
        }
    };
    FrameInterceptorSurface* m_surface;
};

