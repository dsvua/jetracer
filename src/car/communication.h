
#ifndef JETRACER_COMMUNICATION_THREAD_H
#define JETRACER_COMMUNICATION_THREAD_H

#include "Thread.h"
#include "constants.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <iostream>
#include "JHPWMPCA9685.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


namespace Jetracer {

    class communicationThread : public Thread {
    public:
        explicit communicationThread(context_t * ctx)
                                    : _ctx(ctx)
        {
        }
        ~communicationThread() {}
        // rs2::frame_queue depth_queue(CAPACITY);
        // rs2::frame_queue left_ir_queue(CAPACITY);
        // rs2::frame_queue right_ir_queue(CAPACITY);
        // bool sendBuffer(char* buff, const int size);
        // bool receiveBuffer(char* buff, const int size);
        bool readMessage();
        bool sendMessage(const char* buff, const int size);
        bool portIsOpen();
        std::string getMessage();
        bool getIncomingConnection();
        void parseMessage(std::string message);

    private:
        virtual bool threadInitialize();
        virtual bool threadExecute();
        virtual bool threadShutdown();

        int    _socket;
        int    _sd; //socket descriptor
        bool   _portIsOpen;
        int    _speed; // -100..0..100
        int    _steering; // -100..0..100
        std::string _buff;
        bool sendBuffer(const char* buff, const int size);
        bool receiveBuffer(char* buff, const int size);
        void split(const std::string &s, char delim, std::vector<std::string> &elems);
        std::vector<std::string> splitMessage(const std::string &s, char delim);
        context_t * _ctx;

    }; // end of class

    class SpeedUpdateEvent: public BaseEvent {
    public:
        int speed;
    }

    class SteeringUpdateEvent: public BaseEvent {
    public:
        int steering;
    }

    class VideoStreamingEvent: public BaseEvent {
    public:
        bool do_stream;
    }

    typedef std::shared_ptr<VideoFrameEvent> pSpeedUpdateEvent; // p - means pointer
    typedef std::shared_ptr<VideoFrameEvent> pSteeringUpdateEvent; // p - means pointer
    typedef std::shared_ptr<VideoFrameEvent> pVideoStreamingEvent; // p - means pointer

} // end of namespace Jetracer

#endif // JETRACER_COMMUNICATION_THREAD_H
