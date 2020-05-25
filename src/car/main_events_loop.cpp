#include "main_events_loop.h"

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

        // start camera - it should work all times
        jetracer_realsense_camera = new Jetracer::realsenseD435iThread(_ctx);
        jetracer_realsense_camera.initialize();
        jetracer_realsense_camera.waitRunning(_ctx->wait_for_thread);

        // start communication - it should work all times
        jetracer_communication = new Jetracer::communicationThread(_ctx);
        jetracer_communication.initialize();
        jetracer_communication.waitRunning(_ctx->wait_for_thread);

        // start servo control - it should work all times
        jetracer_servo_control = new Jetracer::servoControlThread(_ctx);
        jetracer_servo_control.initialize();
        jetracer_servo_control.waitRunning(_ctx->wait_for_thread);

        return true;
    }


    bool MainEventsLoopThread::threadShutdown() {
        jetracer_video_stream->shutdown();
        jetracer_communication->shutdown();
        jetracer_servo_control->shutdown();
        return true;
    }

    bool MainEventsLoopThread::processEvents(pEvent event) {

        std::unique_lock<std::mutex> lk(m_mutex_subscribers);

        switch (event->event_type) {
            case event_video_streaming_update: {
                if (event->do_stream){
                    startVideo();
                } else {
                    stopVideo();
                }
            }
            else {
                for (auto& subscriber: subscribers[event->event_type]) {
                    std::function<bool(pEvent)> pushEventToSubscriber = subscriber.second;
                    pushEventToSubscriber(event);
                }                
            }
        }

        if (event->event_type == event_stop_thread) {
            stopVideo();
            shutdown();            
        }

        return true;
    }

    // this function is used to compare pthread_t's in map
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

    void MainEventsLoopThread::startVideo(){
        jetracer_video_stream = new videoStreamThread(_ctx);
        jetracer_video_stream->initialize();
        jetracer_video_stream->waitRunning();
    }

    void MainEventsLoopThread::stopVideo(){
        jetracer_video_stream->shutdown();
    }


}