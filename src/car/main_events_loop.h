#ifndef JETRACER_EVENTS_LOOP_THREAD_H
#define JETRACER_EVENTS_LOOP_THREAD_H

#include <librealsense2/rs.hpp>
#include <iostream>

#include "EventsThread.h"
#include "constants.h"
#include "types.h"
#include "realsense_camera.h"
#include "image_pipeline.h"
#include "communication.h"
#include "video_stream.h"
#include "ServoControl.h"

namespace Jetracer {

    class MainEventsLoopThread : public EventsThread {
    public:
        explicit MainEventsLoopThread(context_t* ctx)
                                    : _ctx(ctx) {}
        ~MainEventsLoopThread() {}

        bool subscribeForEvent(EventType _event_type, pthread_t _thread_id, 
            std::function<bool(pEvent)> pushEventCallback);
        bool unSubscribeFromEvent(EventType _event_type, pthread_t _thread_id); 

    protected:
        bool processEvent(pEvent event) override;
        bool threadInitialize() override;
        bool threadShutdown() override;

    private:
        bool pthreadComparison(const pthread_t &a, const pthread_t &b);
        void startVideo();
        void stopVideo();

        context_t* _ctx;
        std::mutex m_mutex_subscribers;
        //example: event_stop_thread, thread_id, callback for pushEvent
        // {EventType: {m_threadID: callback} }
        map<EventType, map < pthread_t, std::function<bool(pEvent)>, pthreadComparison > > subscribers;

        // threads that are started by main loop
        Jetracer::realsenseD435iThread* jetracer_realsense_camera;
        Jetracer::communicationThread* jetracer_communication;
        Jetracer::servoControlThread* jetracer_servo_control;
    };
} // namespace Jetracer

#endif // JETRACER_EVENTS_LOOP_THREAD_H
