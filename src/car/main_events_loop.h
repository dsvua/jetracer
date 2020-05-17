
#ifndef JETRACER_EVENTS_LOOP_THREAD_H
#define JETRACER_EVENTS_LOOP_THREAD_H

#include "Thread.h"
#include "constants.h"
#include "types.h"
#include <librealsense2/rs.hpp>

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

        context_t* _ctx;
        std::mutex m_mutex_subscribers;
        //example: event_stop_thread, thread_id, callback for pushEvent
        // {EventType: {m_threadID: callback} }
        map<EventType, map < pthread_t, std::function<bool(pEvent)>, pthreadComparison > > subscribers;
    };
} // namespace Jetracer

#endif // JETRACER_EVENTS_LOOP_THREAD_H
