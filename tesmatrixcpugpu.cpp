#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>

void matmulCPUThreadedPart(const unsigned int size,
                           const std::vector<float> &A,
                           const std::vector<float> &B,
                           std::vector<float> &C,
                           int startRow, int endRow) {
    for (unsigned int i = startRow; i < endRow; ++i) {
        for (unsigned int j = 0; j < size; ++j) {
            float tmp = 0;
            for (unsigned int k = 0; k < size; ++k) {
                tmp += A[i * size + k] * B[k * size + j];
            }
            C[i * size + j] = tmp;
        }
    }
}

void matmulCPUThreaded(const unsigned int size, const std::vector<float> &A, const std::vector<float> &B, std::vector<float> &C) {
    unsigned int numThreads = std::thread::hardware_concurrency();  // Get the number of hardware threads
    std::vector<std::thread> threads(numThreads);  // Create a vector of threads

    // Divide the work among the threads
    int rowsPerThread = size / numThreads;
    for (unsigned int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? size : (startRow + rowsPerThread);
        threads[i] = std::thread(matmulCPUThreadedPart, size, std::ref(A), std::ref(B), std::ref(C), startRow, endRow);
    }

    // Wait for all threads to complete
    for (std::thread &t : threads) {
        t.join();
    }
}


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

void matmulCPU(const unsigned int size, const std::vector<float> &A, const std::vector<float> &B, std::vector<float> &C) {
    for (unsigned int i = 0; i < size; ++i) {
        for (unsigned int j = 0; j < size; ++j) {
            float tmp = 0;
            for (unsigned int k = 0; k < size; ++k) {
                tmp += A[i*size + k] * B[k*size + j];
            }
            C[i*size + j] = tmp;
        }
    }
}

int main() {
        std::cout << "\033[35m";
        std::cout << R"(


 /$$$$$$$$ /$$      /$$         /$$$$$$$   /$$$$$$  /$$    /$$ /$$$$$$
| $$_____/| $$$    /$$$        | $$__  $$ /$$__  $$| $$   | $$|_  $$_/
| $$      | $$$$  /$$$$        | $$  \ $$| $$  \ $$| $$   | $$  | $$  
| $$$$$   | $$ $$/$$ $$ /$$$$$$| $$$$$$$/| $$$$$$$$|  $$ / $$/  | $$  
| $$__/   | $$  $$$| $$|______/| $$__  $$| $$__  $$ \  $$ $$/   | $$  
| $$      | $$\  $ | $$        | $$  \ $$| $$  | $$  \  $$$/    | $$  
| $$      | $$ \/  | $$        | $$  | $$| $$  | $$   \  $/    /$$$$$$
|__/      |__/     |__/        |__/  |__/|__/  |__/    \_/    |______/
                                                                        
)" << '\n';
    std::cout << "\033[36m";
    std::cout << "FM-RAVI SOFTWARE GPU BENCHMARK \n \n SOFTWARE INI AKAN MEMBANDINGKAN PERFORMA GPU KAMU VS CPU KAMU EHEHE\033[36m \n \n \n";
    std::cout << "\033[32m";
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
        // Print platform info
    std::cout << "Menggunakan " << platforms.size() << " platform hardware\n";
    for (auto &platform : platforms) {
        std::string platformName, platformVendor, platformVersion;
        platform.getInfo(CL_PLATFORM_NAME, &platformName);
        platform.getInfo(CL_PLATFORM_VENDOR, &platformVendor);
        platform.getInfo(CL_PLATFORM_VERSION, &platformVersion);

        std::cout << "Nama Platform: " << platformName << '\n';
        std::cout << "Vendor: " << platformVendor << '\n';
        std::cout << "Versi Platform: " << platformVersion << '\n';

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (auto &device : devices) {
            std::string deviceName, deviceVendor, driverVersion;
            cl_uint numCores;
            cl_ulong memSize;

            device.getInfo(CL_DEVICE_NAME, &deviceName);
            device.getInfo(CL_DEVICE_VENDOR, &deviceVendor);
            device.getInfo(CL_DRIVER_VERSION, &driverVersion);
            device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &numCores);
            device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &memSize);

            std::cout << "  Nama Perangkat: " << deviceName << '\n';
            std::cout << "  Vendor Perangkat: " << deviceVendor << '\n';
            std::cout << "  Versi Driver: " << driverVersion << '\n';
            std::cout << "  Jumlah Core: " << numCores << '\n';
            std::cout << "  Ukuran Memori: " << memSize / (1024 * 1024) << "MB\n\n";
        }
        std::cout << "-------------------------\n";
    }
    cl::Platform platform = platforms[0];
    unsigned int size;  // Declare the size variable

    std::cout << "\033[33m";

    std::cout << "Masukkan ukuran buffer: ";
    std::cin >> size;  // Read the size from user input
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Flush the input buffer


    std::cout << "Menggunakan Buffer Size: " << size << "\n";

    std::cout << "menghitung...\n \n";

    std::vector<float> A(size * size), B(size * size), C(size * size), D(size * size);

    // Fill matrices A and B with random numbers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 100);

    for (unsigned int i = 0; i < size * size; ++i) {
        A[i] = dis(gen);
        B[i] = dis(gen);
    }

    // Perform matrix multiplication on CPU
    auto startCPU = std::chrono::high_resolution_clock::now();
    matmulCPU(size, A, B, C);
    auto stopCPU = std::chrono::high_resolution_clock::now();
    auto durationCPU = std::chrono::duration_cast<std::chrono::microseconds>(stopCPU - startCPU);
    std::cout << "\033[31m";  // Set color to red
    std::cout << "Time taken for matrix multiplication on CPU: " << durationCPU.count() / 1e6 << " seconds" << std::endl;

    // Perform Matrix Multiplication on GPU Multi Threaded
    // Perform matrix multiplication on multi-core CPU
    auto startThreadedCPU = std::chrono::high_resolution_clock::now();
    matmulCPUThreaded(size, A, B, C);
    auto stopThreadedCPU = std::chrono::high_resolution_clock::now();
    auto durationThreadedCPU = std::chrono::duration_cast<std::chrono::microseconds>(stopThreadedCPU - startThreadedCPU);
    std::cout << "\033[34m";
    std::cout << "Time taken for matrix multiplication on multi-core CPU: " << durationThreadedCPU.count() / 1e6 << " seconds" << std::endl;


    // Initialize OpenCL
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

    auto startGPU = std::chrono::high_resolution_clock::now();
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, localSize);
    queue.finish();
    auto stopGPU = std::chrono::high_resolution_clock::now();

    // Read back the result
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(float) * size * size, D.data());

    // Time taken
    auto durationGPU = std::chrono::duration_cast<std::chrono::microseconds>(stopGPU - startGPU);
    std::cout << "\033[35m";
    std::cout << "Time taken for matrix multiplication on GPU: " << durationGPU.count() / 1e6 << " seconds" << std::endl;
    float speedup = static_cast<float>(durationCPU.count()) / static_cast<float>(durationGPU.count());
    float speedup_multicore = static_cast<float>(durationThreadedCPU.count()) / static_cast<float>(durationGPU.count());
    std::cout << "\033[37m";
    std::cout << "Speedup time for GPU vs Single Threaded CPU: " << speedup << "x" << std::endl;
    std::cout << "Speedup time for GPU vs Multi Threaded CPU: " << speedup_multicore << "x" << std::endl;
    std::cout << "\033[0m";
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}
