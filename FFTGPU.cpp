#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
#include <cmath>
#include <CL/cl.hpp>
#include <fstream>

typedef std::complex<double> Complex;

const char *fftKernelCode = R"CLC(
__kernel void fft(__global double2* a, const int n, const int stageSize, const int halfStageSize)
{
    int tid = get_global_id(0);
    int stage = tid / halfStageSize;
    int offset = tid % halfStageSize;

    int start = stage * stageSize + offset;
    int middle = start + halfStageSize;

    double angle = -2 * M_PI * offset / stageSize;
    double2 twiddle = (double2)(cos(angle), sin(angle));

    double2 even = a[start];
    double2 odd = a[middle];

    double2 t = (double2)(odd.x * twiddle.x - odd.y * twiddle.y, odd.x * twiddle.y + odd.y * twiddle.x);

    a[start] = (double2)(even.x + t.x, even.y + t.y);
    a[middle] = (double2)(even.x - t.x, even.y - t.y);
}


)CLC";

int main()
{
    // Original array
    std::vector<Complex> a = {0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 2, 7, 2, 6, 1, 5, 7, 3, 2, 6, 1, 7, 8, 2, 2, 7, 9, 10, 15, 12, 12, 124, 1, 2, 5, 2, 4, 6, 1, 7, 9, 1, 6, 4, 3, 2, 5, 6, 7, 4, 3, 2, 6, 4, 6, 4, 6, 3, 7, 2, 8, 5, 7, 5, 2, 5, 1};
    int original_size = a.size();
    int padded_size = 1;
    while (padded_size < original_size)
    {
        padded_size *= 2;
    }
    a.resize(padded_size, 0);

    // Initialize OpenCL
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms[0];
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];
    cl::Context context(device);
    cl::Program program(context, fftKernelCode);

    cl_int buildErr = program.build("-cl-std=CL1.2");
    if (buildErr != CL_SUCCESS)
    {
        std::cout << "Program build failed. Error code: " << buildErr << std::endl;
        return -1;
    }

    cl::Buffer bufferA(context, CL_MEM_READ_WRITE, sizeof(cl_double2) * a.size());
    cl::CommandQueue queue(context, device);
    queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, sizeof(cl_double2) * a.size(), a.data());

    auto start_cl = std::chrono::high_resolution_clock::now();

    cl::Kernel kernel(program, "fft");
    int n = a.size();
    int numStages = log2(n);
    for (int stage = 0; stage < numStages; ++stage)
    {
        int stageSize = 1 << (stage + 1);
        int halfStageSize = stageSize / 2;
        kernel.setArg(0, bufferA);
        kernel.setArg(1, n);
        kernel.setArg(2, stageSize);
        kernel.setArg(3, halfStageSize);
        cl::NDRange globalSize(n / 2);
        cl::NDRange localSize(256);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, localSize);
    }

    std::ofstream file("gpu_fft_output.csv");

    // Write the header
    file << "Real,Imaginary\n";

    // Timing and execution
    auto stop_cl = std::chrono::high_resolution_clock::now();
    auto duration_cl = std::chrono::duration_cast<std::chrono::microseconds>(stop_cl - start_cl);
    std::cout << "Time taken for FFT on OpenCL: " << duration_cl.count() << " microseconds" << std::endl;

    cl_int err = queue.enqueueReadBuffer(bufferA, CL_TRUE, 0, sizeof(cl_double2) * a.size(), a.data());
    if (err != CL_SUCCESS)
    {
        std::cout << "Failed to read back data from buffer. Error code: " << err << std::endl;
        return -1;
    }

    std::cout << "OpenCL (GPU) Results: \n";
    for (auto &x : a)
    {
        std::cout << x << ' ';
        file << x.real() << ',' << x.imag() << '\n'; // Write to CSV file
    }
    std::cout << '\n';

    // Close the file
    file.close();

    return 0;
}
