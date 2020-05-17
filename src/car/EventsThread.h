#include "Thread.h"
#include "constants.h"
#include "types.h"
#include <librealsense2/rs.hpp>

#ifndef JETRACER_EVENTS_THREAD_H
#define JETRACER_EVENTS_THREAD_H


namespace Jetracer {

    class EventsThread : public Thread {
    public:
        explicit EventsThread() {}
        ~EventsThread() {}
        bool pushEvent(pEvent event);
    protected:
        virtual bool processEvent(pEvent event) = 0;
        virtual bool threadInitialize() = 0;
        virtual bool threadShutdown() = 0;
    private:
        bool threadExecute() final;
        std::queue<pEvent> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_cv;
    };
} // namespace Jetracer

#endif // JETRACER_EVENTS_THREAD_H
