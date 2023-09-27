#include <CL/cl.hpp>
#include <iostream>
#include <vector>

int main() {
    // Get list of OpenCL platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.empty()) {
        std::cerr << "No platforms found. Check OpenCL installation!\n";
        return 1;
    }

    // Print platform info
    std::cout << "Found " << platforms.size() << " platforms\n";
    for (auto &platform : platforms) {
        std::string platformName;
        platform.getInfo(CL_PLATFORM_NAME, &platformName);
        std::cout << "Platform: " << platformName << '\n';
    }

    // Get first platform
    cl::Platform default_platform = platforms[0];

    // Get list of devices
    std::vector<cl::Device> devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if (devices.empty()) {
        std::cerr << "No devices found. Check OpenCL installation!\n";
        return 1;
    }

    // Print device info
    std::cout << "Found " << devices.size() << " devices\n";
    for (auto &device : devices) {
        std::string deviceName;
        device.getInfo(CL_DEVICE_NAME, &deviceName);
        std::cout << "Device: " << deviceName << '\n';
    }

    return 0;
}
