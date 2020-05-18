#include "video_stream.h"

using namespace cv;
using namespace rs2;

namespace Jetracer {

    bool servoControlThread::threadInitialize() {

        // callback for pushEvent
        auto callbackPushEvent = [this](pEvent event){
            this->pushEvent(event);
        }

        // subscribe to events - minimum to shutdown event
        for (auto& event : eventsToSubscribe) {
            _ctx->subscribeForEvent(event, m_threadID, callbackPushEvent);
        }

        // Initialize the PWM board
        pca9685 = new PCA9685() ;
        int err = pca9685->openPCA9685();

        if (err < 0){
            printf("Error: %d", pca9685->error);
        }
        
        printf("PCA9685 Device Address: 0x%02X\n",pca9685->kI2CAddress);
        pca9685->setAllPWM(0,0);
        pca9685->reset();
        pca9685->setPWMFrequency(50);
        // Set the PWM to "neutral" (1.5ms)
        sleep(1);
        pca9685->setPWM(ESC_CHANNEL,0,ESC_PWM_NEUTRAL);
        _speed = 0;

        return true;
    }

    bool servoControlThread::processEvents(pEvent event) {

        if (event->event_type != event_stop_thread) {
            switch (event->event_type) {
                case event_speed_update: {
                    _speed = event->speed;
                    setPwmSpeed();
                }
                case event_steering_update: {
                    _steering = event->steering;
                    setPwmSteering();
                }
            }
        } else {
            shutdown();            
        }
        return true;
    }

    void servoControlThread::setPwmSpeed() {
        int pwmSpeed = 0;
        if (_speed > 0 ){
            pwmSpeed = ESC_PWM_NEUTRAL + ESC_FORWARD_RANGE * _speed / 100;
        } else {
            pwmSpeed = ESC_PWM_NEUTRAL + ESC_REVERSE_RANGE * _speed / 100;
        }
        std::cout << "setPwmSpeed: " << pwmSpeed << std::endl;
        pca9685->setPWM(ESC_CHANNEL,0,pwmSpeed);
    }

    void servoControlThread::setPwmSteering() {
        int pwmSteering = 0;
        if (_steering > 0 ){
            pwmSteering = SERVO_PWM_NEUTRAL + SERVO_RIGHT_RANGE * _steering / 100;
            // pwmSteering = 4096/2 + (4096/2) * _steering / 100;
        } else {
            pwmSteering = SERVO_PWM_NEUTRAL + SERVO_LEFT_RANGE * _steering / 100;
            // pwmSteering = 4096/2 + (4096/2) * _steering / 100;
        }
        std::cout << "setPwmSteering: " << pwmSteering << std::endl;
        pca9685->setPWM(STEERING_CHANNEL,0,pwmSteering);
    }

    bool servoControlThread::threadShutdown() {
        _speed = 0;
        setPwmSpeed();
        for (auto& event : eventsToSubscribe) {
            _ctx->unSubscribeFromEvent(event, m_threadID);
        }
        return true;
    }


} // end of namespace Jetracer
