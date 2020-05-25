
#include "realsense_camera.h"
#include "types.h"
#include <iostream>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API


namespace Jetracer {

    bool realsenseD435iThread::threadInitialize() {
        // callback for new frames
        auto callbackNewFrame = [this](const rs2::frame& frame) {
            if (rs2::frameset fs = frame.as<rs2::frameset>()) {
                // With callbacks, all synchronized streams will arrive in a single frameset
                for (rs2::frame& f : fs)
                    switch (f.get_profile().stream_type()){
                        case RS2_STREAM_INFRARED: {
                            pVideoFrameEvent ir_event = make_shared<VideoFrameEvent>();
                            ir_event->event_type = event_new_ir_frame;
                            ir_event->video_frame = f;
                            this->_ctx->pushEvent(ir_event);
                        }
                        case RS2_STREAM_DEPTH: {
                            pDepthFrameEvent depth_event = make_shared<DepthFrameEvent>();
                            depth_event->event_type = event_new_depth_frame;
                            depth_event->depth_frame = f;
                            this->_ctx->pushEvent(depth_event);
                        }
                    }
            } else {
                // Stream that bypass synchronization (such as IMU) will produce single frames
                switch (frame.get_profile().stream_type()){
                    case RS2_STREAM_GYRO: {
                        pGyroFrameEvent gyro_event = make_shared<GyroFrameEvent>();
                        ir_event->event_type = event_new_gyro_frame;
                        ir_event->motion_frame = frame;
                        this->_ctx->pushEvent(gyro_event);
                    }
                    case RS2_STREAM_ACCEL: {
                        pAccelFrameEvent accel_event = make_shared<AccelFrameEvent>();
                        depth_event->event_type = event_new_accel_frame;
                        depth_event->motion_frame = frame;
                        this->_ctx->pushEvent(accel_event);
                    }
                }
            }

        }

        // callback for pushEvent
        auto callbackPushEvent = [this](pEvent event){
            this->pushEvent(event);
        } 

        _config.enable_stream(RS2_STREAM_DEPTH, _ctx->cam_w, _ctx->cam_h, RS2_FORMAT_Z16, _ctx->fps);
        _config.enable_stream(RS2_STREAM_INFRARED, 1, _ctx->cam_w, _ctx->cam_h, RS2_FORMAT_Y8, _ctx->fps);
        // config.enable_stream(RS2_STREAM_INFRARED, 2, _ctx->cam_w, _ctx->cam_h, RS2_FORMAT_Y8, _ctx->fps);

        // start pipeline
        _pipeline_profile = _pipeline.start(_config, callbackNewFrame);

        // rs2::device selected_device = pipeline_profile.get_device();
        // rs2::depth_sensor depth_sensor = selected_device.first<rs2::depth_sensor>();
        // depth_sensor.set_option(RS2_OPTION_EMITTER_ON_OFF, 1);
        auto stream = _pipeline_profile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
        auto intrinsics = stream.get_intrinsics(); // Calibration data
        _ctx->jetson_camera_intrinsics = intrinsics;

        // subscribe to events - minimum to shutdown event
        for (auto& event_type : eventsToSubscribe) {
            _ctx->subscribeForEvent(event_type, m_threadID, callbackPushEvent);
        }

        std::cout << "Camera thread is initialized" << std::endl;
        return true;
    }

    // bool realsenseD435iThread::threadExecute() {

    //     std::cout << "Camera thread is executing" << std::endl;
    //     // Capture 30 frames to give autoexposure, etc. a chance to settle
    //     for (auto i = 0; i < _ctx->frames_to_skip; ++i) _pipeline.wait_for_frames();
    //     std::cout << "Camera thread skipped first " << _ctx->frames_to_skip << " frames as asked" << std::endl;

    //     while (!m_doShutdown){

    //         // wait for frames and get frameset
    //         rs2::frameset frameset = _pipeline.wait_for_frames();

    //         // get single frame from frameset
    //         rs2::depth_frame depth_frame = frameset.get_depth_frame();
    //         // int emitter_mode, frame_count;
    //         // frame_count = depth_frame.get_frame_metadata(RS2_FRAME_METADATA_FRAME_COUNTER);
    //         // cout << "Emitter mode: " << emitter_mode << "frame count: " << frame_count << endl;

    //         _ctx->depth_queue->enqueue(frameset.get_depth_frame());
    //         _ctx->left_ir_queue->enqueue(frameset.get_infrared_frame(1));
    //     }

    //     return true;
    // }

    bool realsenseD435iThread::threadShutdown() {
        // unsubscribe from events
        for (auto& event_type : eventsToSubscribe) {
            _ctx->unSubscribeFromEvent(event_type, m_threadID);
        }
        return true;
    }

    bool realsenseD435iThread::processEvents(pEvent event) {

        if (event->event_type != event_stop_thread) {
        } else {
            shutdown();            
        }
        return true;
    }

}