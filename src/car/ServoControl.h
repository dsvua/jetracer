
#ifndef JETRACER_SERVO_CONTROL_THREAD_H
#define JETRACER_SERVO_CONTROL_THREAD_H

#include "EventsThread.h"
#include "constants.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "JHPWMPCA9685.h"


namespace Jetracer {

    class servoControlThread : public EventsThread {
    public:
        explicit servoControlThread(context_t * ctx)
                                    : _ctx(ctx)
        {
        }
        ~servoControlThread() {}

    private:
        virtual bool threadInitialize();
        virtual bool threadShutdown();
        virtual bool processEvent(pEvent event);

        int    _speed; // -100..0..100
        int    _steering; // -100..0..100
        context_t * _ctx;
        PCA9685 *pca9685;
        EventType eventsToSubscribe[] = {event_stop_thread, event_speed_update, event_steering_update};

    }; // end of class
} // end of namespace Jetracer

#endif // JETRACER_SERVO_CONTROL_THREAD_H
