#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>

const char *kernelSource = R"CLC(
    __kernel void matmul(const unsigned int size,
                         __global float* A,
                         __global float* B,
                         __global float* C) {
        int i = get_global_id(0);
        int j = get_global_id(1);
        float tmp = 0;
        for(int k=0; k<size; k++) {
            tmp += A[i*size + k] * B[k*size + j];
        }
        C[i*size + j] = tmp;
    }
)CLC";

int main() {
    const unsigned int size = 512;  // Size of the matrix (size x size)
    std::vector<float> A(size * size), B(size * size), C(size * size);

    // Fill matrices A and B with random numbers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 100);

    for (unsigned int i = 0; i < size * size; ++i) {
        A[i] = dis(gen);
        B[i] = dis(gen);
    }

    // Initialize OpenCL
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms[0];
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];
    cl::Context context({device});
    cl::Program program(context, kernelSource, true);
    cl::Kernel kernel(program, "matmul");
    cl::CommandQueue queue(context, device);

    // Create buffers
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY, sizeof(float) * size * size);
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY, sizeof(float) * size * size);
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(float) * size * size);

    // Copy matrices to the device
    queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, sizeof(float) * size * size, A.data());
    queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, sizeof(float) * size * size, B.data());

    // Set kernel arguments and run the kernel
    kernel.setArg(0, size);
    kernel.setArg(1, bufferA);
    kernel.setArg(2, bufferB);
    kernel.setArg(3, bufferC);

    cl::NDRange globalSize(size, size);
    cl::NDRange localSize(16, 16);

    auto start = std::chrono::high_resolution_clock::now();
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, localSize);
    queue.finish();
    auto stop = std::chrono::high_resolution_clock::now();

    // Read back the result
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(float) * size * size, C.data());

    // Time taken
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken for matrix multiplication: " << duration.count() / 1e6 << " seconds" << std::endl;

    return 0;
}
