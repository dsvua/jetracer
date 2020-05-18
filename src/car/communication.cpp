
#include "communication.h"
using namespace std;

namespace Jetracer {
    bool communicationThread::threadInitialize() {

        // start listening on given network port
        struct sockaddr_in addr;
        _socket = socket(AF_INET, SOCK_STREAM, 0);
         
        if (_socket < 0){
            printf("socket error №%i\n",errno);
            return false;
        }
         
        printf("sock= %i \n", _socket);
        memset(&addr, 0, sizeof(addr));
        addr.sin_family=AF_INET;
        addr.sin_port=htons(_ctx->listen_port);
        addr.sin_addr.s_addr= htonl(INADDR_ANY);
        const int trueFlag = 1;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0)
            printf("Failure");
        if (bind(_socket,(struct sockaddr *)&addr,sizeof(addr)) == -1){
            printf("connect error  %i\n",errno);
            return false;
        }
         
        if(listen(_socket, 2) == -1){
            printf("listen() error!");
            return false;
        }

        _portIsOpen = true;
        return true;
    }

    bool communicationThread::threadExecute() {
        while (!m_doShutdown){
            printf("Creating connection\n");
            getIncomingConnection();

            pVideoStreamingEvent video_event = make_shared<VideoStreamingEvent>();
            video_event->event_type = event_video_streaming_update;
            video_event->do_stream = true;
            _ctx->pushEvent(video_event);

            while(!m_doShutdown && readMessage()){
                printf("Getting message\n");
                string message = getMessage();
                printf("Parsing message\n");
                parseMessage(message);
                printf("Message:%s\n", message.c_str());
               
            }
            pVideoStreamingEvent video_event = make_shared<VideoStreamingEvent>();
            video_event->event_type = event_video_streaming_update;
            video_event->do_stream = false;
            _ctx->pushEvent(video_event);

        }
        pVideoStreamingEvent video_event = make_shared<VideoStreamingEvent>();
        video_event->event_type = event_video_streaming_update;
        video_event->do_stream = false;
        _ctx->pushEvent(video_event);
        return true;
    }

    bool communicationThread::portIsOpen(){
        return _portIsOpen;
    }


    bool communicationThread::sendBuffer(const char* buff, const int size){
        int sentSize = 0;
        while (sentSize < size){
            int len = write(_sd, buff + sentSize, size - sentSize);
            if (len < 0){
                printf("%s", strerror(errno));
                return false;
            }
            sentSize += len;
        }
        return true;
    }

    bool communicationThread::receiveBuffer(char* buff, const int size){
        int receivedSize = 0;
        while (receivedSize < size){
            int len = read(_sd, buff + receivedSize, size - receivedSize);
            if (len < 0){
                printf("receiveBuffer error %s", strerror(errno));
                return false;
            }
            if (len == 0){
                printf("Empty packet");
                break;
            }
            // printf("Received receiveBuffer size %i\n", len);
            receivedSize += len;
        }
        // printf("Size %i, receivedSize %i\n", size, receivedSize);
        return size == receivedSize;
    }


    bool communicationThread::sendMessage(const char* buff, const int size){
        char messageSizeStr[5] = {0};
        sprintf(messageSizeStr, "%d", size);
        printf("%d", size);
        sendBuffer(messageSizeStr, 2);
        return sendBuffer(buff, size);

    }

    bool communicationThread::readMessage(){
        char sizeBuff[5] = {0};
        if (receiveBuffer(sizeBuff, 2)){
            printf("Received readMessage %s\n", sizeBuff);
        } else return false;
        int size = atoi(sizeBuff);
        char* buff = new char[size];
        _buff = "";
        
        if (receiveBuffer(buff, size)){
            _buff = string(buff, size);
            delete[] buff;
            return true;
        }
        delete[] buff;
        return false;
    }

    std::string communicationThread::getMessage(){
        return _buff;
    }

    bool communicationThread::getIncomingConnection(){
        _sd = accept(_socket,NULL,NULL);
        
        //----------------------
        struct sockaddr     address;
        socklen_t           addresslen;
        struct sockaddr_in* addrInternet;
        std::string         ip;
        int port;
        char caddr[INET_ADDRSTRLEN];

        addresslen = sizeof(address);
        getpeername(_sd, &address, &addresslen);
        addrInternet = (struct sockaddr_in*)&address;
        
        port = ntohs(addrInternet->sin_port);
        inet_ntop(AF_INET, &(addrInternet->sin_addr), caddr, INET_ADDRSTRLEN);
        ip = inet_ntoa(addrInternet->sin_addr);
        
        //std::copy(std::begin(ipstr), std::end(ipstr), std::begin(client_ipstr_));
        printf("Peer IP address ntoa: %s\n", ip.c_str());
        printf("Peer IP address ntop: %s\n", caddr);
        printf("Peer port           : %d\n", port);
        //----------------------
       
        
        if (_sd == -1) {
            printf("error accept %i\n",errno);
            return false;
        }
        printf("Incoming socket %i\n", _sd);

        _ctx->client_ip_address = ip;
        // _ctx->client_port = port;
        return true;

    }

    void communicationThread::parseMessage(std::string message){
        std::vector<std::string> splitted_message = splitMessage(message, ':');
        int value = atoi(splitted_message[1].c_str());
        //value = atoi(strtok(cmessage, ":"));
        //value = atoi(cmessage);
        string command = splitted_message[0];
        cout << message << "|" << splitted_message[0] << " " << value << "|" << splitted_message[1] << endl;
        if (command == "Forward"){
            value = 1;
            _speed += value;
            if (_speed > 100){
                _speed = 100;
            }

            pSpeedUpdateEvent speed_event = make_shared<SpeedUpdateEvent>();
            speed_event->event_type = event_speed_update;
            speed_event->speed = _speed;
            _ctx->pushEvent(speed_event);

        } else if (command == "Backward"){
            value = 1;
            _speed -= value;
            if (_speed < -100) {
                 _speed = -100;
            }

            pSpeedUpdateEvent speed_event = make_shared<SpeedUpdateEvent>();
            speed_event->event_type = event_speed_update;
            speed_event->speed = _speed;
            _ctx->pushEvent(speed_event);

        } else if (command == "Right"){
            _steering = value;
            if (_steering > 100) {
                _steering = 100;
            }

            pSteeringUpdateEvent steering_event = make_shared<SteeringUpdateEvent>();
            steering_event->event_type = event_steering_update;
            steering_event->steering = _steering;
            _ctx->pushEvent(steering_event);

        } else if (command == "Left"){
            _steering = 0 - value;
            if (_steering < -100) {
                _steering = -100;
            }

            pSteeringUpdateEvent steering_event = make_shared<SteeringUpdateEvent>();
            steering_event->event_type = event_steering_update;
            steering_event->steering = _steering;
            _ctx->pushEvent(steering_event);

        }
        //delete cmessage;

    }

    void communicationThread::split(const string &s, char delim, vector<string> &elems){
        stringstream ss;
        ss.str(s);
        string item;
        while (getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }

    vector<string> communicationThread::splitMessage(const string &s, char delim){
        vector<string> elems;
        split(s, delim, elems);
        return elems;
    }

    bool communicationThread::threadShutdown(){
        return true;
    }


}