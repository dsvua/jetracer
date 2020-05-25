#ifndef JETRACER_TYPES_THREAD_H
#define JETRACER_TYPES_THREAD_H

#include "constants.h"
#include "Ordered.h"
#include <cuda.h>
#include <librealsense2/rs.hpp>
#include <functional>
#include <pthread.h>

namespace Jetracer {

    typedef struct
    {
        int cam_w = 848;
        int cam_h = 480;
        int fps = 90;
        int frames_to_skip = 30; // discard all frames until start_frame to
                                 // give autoexposure, etc. a chance to settle
        int left_gap = 60;  // ignore left 60 pixels on depth image as they
                            // usually have 0 distance and useless
        int bottom_gap = 50; // ignore bottom 50 pixels on depth image
        // unsigned int bottom_gap = 50; // ignore bottom 50 pixels on depth image

        int min_obstacle_height = 5; // ignore obstacles lower then 5mm
        int max_obstacle_height = 250;  // ignore everything higher then 25cm
                                        // as car is not that tall

        rs2_intrinsics jetson_camera_intrinsics;

        Ordered<bool>* stream_video; // by default do not stream video
        Ordered<bool>* self_drive; // by default use remote commands
        std::string client_ip_address; // address of desktop/laptop that controls car
        int client_port = 9000; // port for video streaming/gstreamer that listens on control host
        int listen_port = 5000; // port to listen for commands
        int wait_for_thread = 1 * 1000000; // wait for 1 sec for thread to start

        std::function<bool(pEvent)> pushEvent;
        std::function<bool(EventType, pthread_t, std::function<bool(pEvent)>)> subscribeForEvent;
        std::function<bool(EventType, pthread_t)> unSubscribeFromEvent;

    } context_t;

    enum class EventType {
        event_new_ir_frame,
        event_stream_ir_frame_on,
        event_stream_ir_frame_off,
        event_new_depth_frame,
        event_new_gyro_frame,
        event_new_accel_frame,
        event_speed_update,
        event_steering_update,
        event_video_streaming_update,

        event_stop_thread,
    }

    class BaseEvent {
    public:
        EventType event_type;
    }

    typedef std::shared_ptr<BaseEvent> pEvent; // p - means pointer


} // namespace Jetracer

#endif // JETRACER_TYPES_THREAD_H
