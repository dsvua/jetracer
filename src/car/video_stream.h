#ifndef JETRACER_VIDEO_STREAM_THREAD_H
#define JETRACER_VIDEO_STREAM_THREAD_H

#include <opencv2/videoio.hpp>
#include "EventsThread.h"
#include "constants.h"
#include "types.h"
#include "realsense_camera.h"
#include <fmt/format.h>
#include <iostream>
#include <librealsense2/rs.hpp>

namespace Jetracer {

    class videoStreamThread : public EventsThread {
    public:
        explicit videoStreamThread(context_t * ctx)
                                    : _ctx(ctx)
        {
        }
        ~videoStreamThread() {}
        // rs2::frame_queue depth_queue(CAPACITY);
        // rs2::frame_queue left_ir_queue(CAPACITY);
        // rs2::frame_queue right_ir_queue(CAPACITY);
    private:
        virtual bool threadInitialize();
        virtual bool threadShutdown();
        virtual bool processEvent(pEvent event);
        context_t * _ctx;
        cv::VideoWriter video_writer;
        EventType eventsToSubscribe[] = {event_stop_thread, event_new_ir_frame};

    }; // end of class
} // end of namespace Jetracer

#endif // JETRACER_VIDEO_STREAM_THREAD_H
