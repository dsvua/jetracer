
all:
	nvcc -std=c++11 -Xcompiler -pthread cuda/cuda-pointcloud.cu main.cpp realsense_camera.cpp Thread.cpp video_stream.cpp JHPWMPCA9685.cpp communication.cpp -o ../../build/jetracer -I/usr/include `pkg-config cuda-10.0 realsense2 opencv4 --cflags --libs` -lfmt -li2c
# 	g++ -std=c++11 -pthread main.cpp realsense_camera.cpp Thread.cpp video_stream.cpp JHPWMPCA9685.cpp communication.cpp -o ../../build/jetracer -I/usr/include `pkg-config cuda-10.1 realsense2 opencv430 --cflags --libs` -lfmt -li2c

# cuda_pointcloud:
# 	nvcc -c cuda/cuda-pointcloud.cu -Icuda -L/usr/local/cuda/lib64

