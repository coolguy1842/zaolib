#include "./device.hpp"

int Device::initDevice() {
    hid_device_info* devices = hid_enumerate(VENDOR_ID, PRODUCT_ID);
    hid_device_info* current_device = devices;

    while(current_device) {
        if(USAGE_PAGE == 0x00 && USAGE == 0x00) {
            device = hid_open_path(current_device->path);

            memset(device_path, '\0', sizeof(device_path));
            strcpy(device_path, current_device->path);

            break;
        }

        if(current_device->usage_page == USAGE_PAGE && current_device->usage == USAGE) {
            device = hid_open_path(current_device->path);

            memset(device_path, '\0', sizeof(device_path));
            strcpy(device_path, current_device->path);

            break;
        }

        current_device = current_device->next;
    }

    hid_free_enumeration(devices);

    if(!device) {
        device = NULL;

        return -1;
    }

    return 0;
}

Device::Device(unsigned int VENDOR_ID, unsigned int PRODUCT_ID, unsigned int usage_page, unsigned int usage, std::function<void()> onDeviceConnect) :
    onDeviceConnect(onDeviceConnect), VENDOR_ID(VENDOR_ID), PRODUCT_ID(PRODUCT_ID), USAGE_PAGE(usage_page), USAGE(usage) {
    initDevice();

    if(device) {
        this->onDeviceConnect();
    }
    else {
        printf("Failed to open HID device %.4X:%.4X\n", VENDOR_ID, PRODUCT_ID);
    }
}

Device::~Device() {
    if(device) {
        hid_close(device);
    }
}


int Device::sendPacket(std::vector<unsigned char> data) {
    return hid_write(device, data.data(), data.size());
}

std::wstring Device::getError() {
    return std::wstring(hid_error(device));
}
