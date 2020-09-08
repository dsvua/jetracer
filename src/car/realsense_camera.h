#include "EventsThread.h"
#include "constants.h"
#include "types.h"
#include <librealsense2/rs.hpp>

#ifndef JETRACER_REALSENSE_D435I_THREAD_H
#define JETRACER_REALSENSE_D435I_THREAD_H


namespace Jetracer {

    class realsenseD435iThread : public EventsThread {
    public:
        explicit realsenseD435iThread(context_t * ctx)
                                    : _ctx(ctx)
        {
        }
        ~realsenseD435iThread() {}
    private:
        virtual bool threadInitialize();
        virtual bool threadShutdown();
        virtual bool processEvent(pEvent event);

        context_t * _ctx;
        rs2::config _config;
        rs2::pipeline _pipeline;
        rs2::pipeline_profile _pipeline_profile;
        EventType eventsToSubscribe[] = {event_stop_thread};
    };

    class VideoFrameEvent: public BaseEvent {
    public:
        rs2::video_frame video_frame = rs2::frame{};
    };

    class DepthFrameEvent: public BaseEvent {
    public:
        rs2::depth_frame depth_frame = rs2::frame{};
    };

    class GyroFrameEvent: public BaseEvent {
    public:
        rs2::motion_frame motion_frame = rs2::frame{};
    };

    class AccelFrameEvent: public BaseEvent {
    public:
        rs2::motion_frame motion_frame = rs2::frame{};
    };

    typedef std::shared_ptr<VideoFrameEvent> pVideoFrameEvent; // p - means pointer
    typedef std::shared_ptr<DepthFrameEvent> pDepthFrameEvent; // p - means pointer
    typedef std::shared_ptr<GyroFrameEvent>  pGyroFrameEvent; // p - means pointer
    typedef std::shared_ptr<AccelFrameEvent> pAccelFrameEvent; // p - means pointer

} // namespace Jetracer

#endif // JETRACER_REALSENSE_D435I_THREAD_H
