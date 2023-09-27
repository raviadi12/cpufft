#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <CL/cl.hpp>
#include <cmath>

#define MAX_BUFFER_SIZE 16  // Set this to your maximum expected size

const char *kernelSource = R"CLC(
    #define MAX_BUFFER_SIZE 16  // Set this to your maximum expected size

    int my_strcmp(const char *s1, __global const char *s2) {
        unsigned char c1, c2;
        while (*s1 && (*s1 == *s2)) {
            s1++;
            s2++;
        }
        c1 = (unsigned char) *s1;
        c2 = (unsigned char) *s2;
        return c1 - c2;
    }

    void my_strcpy(__global char *dest, const char *src) {
        while ((*dest++ = *src++));
    }

    __kernel void brute_force(const unsigned int targetLen,
                              __global const char *charset,
                              const unsigned int charsetLen,
                              __global const char *target,
                              __global char *result,
                              const unsigned int offset) {
        int gid = get_global_id(0) + offset;
        char buffer[MAX_BUFFER_SIZE];
        unsigned int temp = gid;

        for (int i = 0; i < targetLen; ++i) {
            buffer[i] = charset[temp % charsetLen];
            temp /= charsetLen;
        }

        buffer[targetLen] = '\0';

        if (my_strcmp(buffer, target) == 0) {
            my_strcpy(result, buffer);
        }
    }
)CLC";

int main() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms[0];
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];
    cl::Context context({device});
    cl::Program program(context, kernelSource, true);

    if (program.build({device}) != CL_SUCCESS) {
        std::cout << "Error during compilation!" << std::endl;
        std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        std::cerr << "Build log:\n" << buildLog << std::endl;
        return 1;
    }

    cl::Kernel kernel(program, "brute_force");
    cl::CommandQueue queue(context, device);

    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string target = "LumarDI";  // You can change this to test different lengths

    cl::Buffer bufferCharset(context, CL_MEM_READ_ONLY, charset.size());
    cl::Buffer bufferTarget(context, CL_MEM_READ_ONLY, target.size() + 1);
    cl::Buffer bufferResult(context, CL_MEM_WRITE_ONLY, MAX_BUFFER_SIZE);

    std::vector<char> emptyResult(MAX_BUFFER_SIZE, 0);
    queue.enqueueWriteBuffer(bufferResult, CL_TRUE, 0, MAX_BUFFER_SIZE, emptyResult.data());
    queue.enqueueWriteBuffer(bufferCharset, CL_TRUE, 0, charset.size(), charset.data());
    queue.enqueueWriteBuffer(bufferTarget, CL_TRUE, 0, target.size() + 1, target.c_str());

    kernel.setArg(0, static_cast<unsigned int>(target.size()));
    kernel.setArg(1, bufferCharset);
    kernel.setArg(2, static_cast<unsigned int>(charset.size()));
    kernel.setArg(3, bufferTarget);
    kernel.setArg(4, bufferResult);

    size_t maxThreads=1e8;
    std::cout << "Device Max Threads: " << maxThreads << std::endl;
    
    size_t globalSize = static_cast<size_t>(std::pow(charset.size(), target.size()));
    size_t numLaunches = (globalSize + maxThreads - 1) / maxThreads;

    char result[MAX_BUFFER_SIZE] = {0};  // Initialize to zero
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numLaunches; ++i) {
        size_t offset = i * maxThreads;
        size_t currentGlobalSize = std::min(maxThreads, globalSize - offset);

        // Set the offset as an additional kernel argument
        kernel.setArg(5, static_cast<unsigned int>(offset));

        cl_int err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(currentGlobalSize), cl::NullRange);
        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueuing kernel: " << err << std::endl;
            return 1;
        }

        queue.finish();

        // Read the result buffer
        err = queue.enqueueReadBuffer(bufferResult, CL_TRUE, 0, MAX_BUFFER_SIZE, result);
        if (err != CL_SUCCESS) {
            std::cerr << "Error reading buffer: " << err << std::endl;
            return 1;
        }

        // Check if password is found
        if (result[0] != '\0') {
            break;
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();

    if (result[0] != '\0') {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        double elapsedTime = duration.count() / 1e6;
        std::cout << "The time it takes to Brute Force " << result << " is " << elapsedTime << " seconds" << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
        std::cout << "Password not found" << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return 0;
}

