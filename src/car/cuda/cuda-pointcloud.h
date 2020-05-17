#ifndef JETRACER_CUDA_POINTCLOUD_H
#define JETRACER_CUDA_POINTCLOUD_H

#include <librealsense2/rs.hpp>
#include "assert.h"

// CUDA headers
#include <cuda_runtime.h>

#define RS2_CUDA_THREADS_PER_BLOCK 256

namespace Jetracer {
    void deproject_depth_cuda(float * points, const rs2_intrinsics& intrin, const uint16_t * depth);
}


#endif // JETRACER_CUDA_POINTCLOUD_H