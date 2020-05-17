
#include "EventsThread.h"
#include <iostream>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API


namespace Jetracer {



    // bool EventsThread::threadInitialize() {
    //     // auto callback = [&](const rs2::frame& frame){
    //     // auto callback = [this](const rs2::frame& frame){
    //     //     this->pushMsg();
    //     // }
    //     // rs2::pipeline_profile profiles = pipe.start(callback);



    //     return true;
    // }

    bool EventsThread::threadExecute() {
        while (!m_doShutdown)
        {
            pEvent event = 0;
            {
                // Wait for a message to be added to the queue
                std::unique_lock<std::mutex> lk(m_mutex);
                while (m_queue.empty()) {
                    m_cv.wait(lk);
                }

                if (m_queue.empty())
                    continue;

                event = m_queue.front();
                m_queue.pop();
            }
            processEvent(event);
        }

        return true;
    }

    // bool EventsThread::threadShutdown() {
    //     return true;
    // }

    bool EventsThread::pushEvent(pEvent event) {
        // Add event to queue and notify threadExecute
        std::unique_lock<std::mutex> lk(m_mutex);
        m_queue.push(event);
        m_cv.notify_one();
    }
}