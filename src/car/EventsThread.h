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
    protected:
        bool threadExecute() final;
        bool threadInitialize() override;
        bool threadShutdown() override;
    private:
        context_t * _ctx;
        rs2::config _config;
        rs2::pipeline _pipeline;
        rs2::pipeline_profile _pipeline_profile;
    };
} // namespace Jetracer

#endif // JETRACER_EVENTS_THREAD_H
