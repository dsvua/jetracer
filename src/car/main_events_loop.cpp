
#include "main_events_loop.h"
#include <iostream>
#include "realsense_camera.h"
#include "image_pipeline.h"
#include "communication.h"

namespace Jetracer {

    bool MainEventsLoopThread::threadInitialize() {
        // pushing callbacks
        auto pushEventCallback = [this](pEvent event)
        {
            this->pushEvent(event);
        }
        _ctx->pushEvent = pushEventCallback;

        auto subscribeForEventCallback = [this](EventType _event_type, 
                        pthread_t _thread_id, 
                        std::function<bool(pEvent)> pushEventCallback)
        {
            this->subscribeForEvent(_event_type, _thread_id, pushEventCallback);
        }
        _ctx->subscribeForEvent = subscribeForEventCallback;

        auto unSubscribeFromEventCallback = [this](EventType _event_type, 
                        pthread_t _thread_id)
        {
            this->unSubscribeFromEvent(_event_type, _thread_id);
        }
        _ctx->unSubscribeFromEvent = unSubscribeFromEventCallback;

        // start image processing pipeline
        Jetracer::imagePipelineThread jetracer_image_pipeline(_ctx);
        jetracer_image_pipeline.initialize();
        jetracer_image_pipeline.waitRunning(_ctx->wait_for_thread);
        return true;
    }


    bool MainEventsLoopThread::threadShutdown() {
        return true;
    }

    bool processEvents(pEvent event) {

        switch (event->event_type) {
            case event_stop_thread: {
                shutdown();
            }
        }
        return true;
    }

    // this function is used to compare pthread_t ids in map
    bool MainEventsLoopThread::pthreadComparison(const pthread_t &a, const pthread_t &b)
    {
        return memcmp(&a, &b, sizeof(pthread_t)) < 0;
    }

    bool MainEventsLoopThread::subscribeForEvent(EventType _event_type, 
                        pthread_t _thread_id, 
                        std::function<bool(pEvent)> pushEventCallback
                        ) {
        std::unique_lock<std::mutex> lk(m_mutex_subscribers);
        subscribers[_event] = {_thread_id, pushEventCallback};
        return true
    }

    bool MainEventsLoopThread::unSubscribeFromEvent(EventType _event_type, 
                        pthread_t _thread_id,
                        ) {
        std::unique_lock<std::mutex> lk(m_mutex_subscribers);
        subscribers[_event].erase(_thread_id);
        return true
    }

}