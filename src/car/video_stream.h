#ifndef JETRACER_VIDEO_STREAM_THREAD_H
#define JETRACER_VIDEO_STREAM_THREAD_H


namespace Jetracer {

    class videoStreamThread : public Thread {
    public:
        explicit videoStreamThread(context_t * ctx)
                                    : _ctx(ctx)
        {
        }
        ~videoStreamThread() {}
        // rs2::frame_queue depth_queue(CAPACITY);
        // rs2::frame_queue left_ir_queue(CAPACITY);
        // rs2::frame_queue right_ir_queue(CAPACITY);
    private:
        virtual bool threadInitialize();
        virtual bool threadExecute();
        virtual bool threadShutdown();
        context_t * _ctx;

    } // end of class
} // end of namespace Jetracer

#endif // JETRACER_VIDEO_STREAM_THREAD_H