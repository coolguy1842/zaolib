#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include <string.h>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <stdexcept>
#include <mutex>

#include <hidapi/hidapi.h>
#include <stdint.h>

class Device {
protected:
    std::function<void()> onDeviceConnect;

    std::mutex deviceMutex;
    hid_device* device;
    char device_path[512];

    int initDevice();
public:
    const unsigned int VENDOR_ID;
    const unsigned int PRODUCT_ID;

    const unsigned int USAGE_PAGE;
    const unsigned int USAGE;

    Device(unsigned int VENDOR_ID, unsigned int PRODUCT_ID, unsigned int usage_page, unsigned int usage, std::function<void()> onDeviceConnect);
    ~Device();

    int sendPacket(std::vector<unsigned char> data);
    std::wstring getError();
};

#endif
