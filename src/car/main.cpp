
#include "types.h"
#include "realsense_camera.h"
#include "image_pipeline.h"
#include "communication.h"
#include <iostream>
#include <signal.h>
#include <opencv2/core.hpp>

using namespace std;
// using namespace cv;

static bool quit = false;

static void signal_handle(int signum)
{
    printf("Quit due to exit command from user!\n");
    quit = true;
}

Jetracer::context_t read_configuration(std::string filename){
    cv::FileStorage fs_config(filename, cv::FileStorage::READ);
    Jetracer::context_t ctx;
    fs_config["camera_width"] >> ctx.cam_w;
    fs_config["camera_height"] >> ctx.cam_h;
    fs_config["camera_fps"] >> ctx.fps;
    fs_config["frames_to_skip"] >> ctx.frames_to_skip;
    fs_config["left_gap"] >> ctx.left_gap;
    fs_config["bottom_gap"] >> ctx.bottom_gap;
    fs_config["min_obstacle_height"] >> ctx.min_obstacle_height;
    fs_config["max_obstacle_height"] >> ctx.max_obstacle_height;
    fs_config["client_port"] >> ctx.client_port;
    fs_config["listen_port"] >> ctx.listen_port;
    fs_config["wait_for_thread"] >> ctx.wait_for_thread;
    fs_config.release();

    return ctx;
}

void write_configuration(std::string filename, Jetracer::context_t ctx){
    cv::FileStorage fs_config(filename, cv::FileStorage::WRITE);
    fs_config << "camera_width" << ctx.cam_w;
    fs_config << "camera_height" << ctx.cam_h;
    fs_config << "camera_fps" << ctx.fps;
    fs_config << "frames_to_skip" << ctx.frames_to_skip;
    fs_config << "left_gap" << ctx.left_gap;
    fs_config << "bottom_gap" << ctx.bottom_gap;
    fs_config << "min_obstacle_height" << ctx.min_obstacle_height;
    fs_config << "max_obstacle_height" << ctx.max_obstacle_height;
    fs_config << "client_port" << ctx.client_port;
    fs_config << "listen_port" << ctx.listen_port;
    fs_config << "wait_for_thread" << ctx.wait_for_thread;
    fs_config.release();

}



int main(int argc, char * argv[]) {
    const std::string inputSettingsFile = argc > 1 ? argv[1] : "configuration.yaml";
    Jetracer::context_t ctx = read_configuration(inputSettingsFile);
    // Jetracer::context_t ctx;
    // write_configuration(inputSettingsFile, ctx);
    std::cout << "listening on port: " << ctx.listen_port << std::endl;

    // ctx.depth_queue = new rs2::frame_queue(CAPACITY);
    // ctx.left_ir_queue = new rs2::frame_queue(CAPACITY);
    ctx.stream_video = new Jetracer::Ordered<bool>(false); // do not stream video by default

    /* Register a shuwdown handler to ensure
       a clean shutdown if user types <ctrl+c> */
    struct sigaction sig_action;
    sig_action.sa_handler = signal_handle;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    sigaction(SIGINT, &sig_action, NULL);

    // start main events loop
    Jetracer::MainEventsLoopThread jetracer_events_loop(&ctx);
    jetracer_events_loop.initialize();
    jetracer_events_loop.waitRunning(ctx.wait_for_thread);


    std::cout << "entering infinite loop" << std::endl;
    while(!quit){
        sleep(1);
    }

    // caught CTRL+C
    std::cout << "caught CTRL+C" << std::endl;
    std::cout << "Closing thread: jetracer_communication_thread" << std::endl;
    jetracer_events_loop.shutdown();
}

// image ignore area: 60px on left and 50px on bottom. 0,0 is top left corner

// plane formula: ax + by + c = z
// fit plane to points: 
// Python example
// # do fit
// tmp_A = []
// tmp_b = []
// for i in range(len(xs)):
//     tmp_A.append([xs[i], ys[i], 1])
//     tmp_b.append(zs[i])
// b = np.matrix(tmp_b).T
// A = np.matrix(tmp_A)
// fit = (A.T * A).I * A.T * b
