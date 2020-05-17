
#include "EventsThread.h"
#include <iostream>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API


namespace Jetracer {



    bool EventsThread::threadInitialize() {
        // auto callback = [&](const rs2::frame& frame){
        auto callback = [this](const rs2::frame& frame){
            this->pushMsg();
        }

        rs2::pipeline_profile profiles = pipe.start(callback);
        return true;
    }

    bool EventsThread::threadExecute() {

        return true;
    }

    bool EventsThread::threadShutdown() {
        return true;
    }

    bool EventsThread::pushMsg() {

    }
}