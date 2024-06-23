#include "./Z2.hpp"

template<typename T>
T map(T x, T in_min, T in_max, T out_min, T out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void printBytes(std::vector<unsigned char> bytes) {
    for(unsigned char byte : bytes) {
        printf("0x%02X ", byte);
    }

    printf("\n");
}


std::vector<unsigned char> Z2::getFlashData(unsigned char commandID) {
    std::vector<unsigned char> buf(17);
    int ret = sendPacket(initialPacket);
    if(ret < 0) { };

    hid_read(device, buf.data(), buf.size());

    ret = sendPacket({
        0x08, UsbCommandID::ReadFlashData, 0x00, 0x00, commandID, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(0x3B - commandID)
    });

    if(ret < 0) {
        return { };
    }

    hid_read(device, buf.data(), buf.size());

    std::vector<unsigned char> out(10);
    memmove(out.data(), buf.data() + 6, 10);
    return out;
}


int Z2::setDPIProfile(unsigned char profile) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x04, 0x02, profile, (unsigned char)(0x55 - profile), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xEB
    });
}

int Z2::setDPIProfilesCount(unsigned char count) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    ret = sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x02, 0x02, count, (unsigned char)(0x55 - count), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xED
    });

    return ret;
}

int Z2::setProfileDPI(unsigned char profile, unsigned int DPI) {
    if(DPI > 26000) return -2;
    else if(DPI <= 0) return -3;

    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    const unsigned int scaledDPI = std::ceil(DPI / 50.0f) - 1;
    const unsigned char overflowByte = 0x44 * (unsigned char)std::floor(scaledDPI / 256.0f);

    const unsigned char firstByte = (unsigned char)scaledDPI;
    const unsigned char secondByte = (0x55 - overflowByte) - (firstByte * 2);

    const unsigned char profileScaled = profile * 4;
    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, (unsigned char)(0x0C + profileScaled), 0x04, firstByte, firstByte, overflowByte, secondByte, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(0xE1 - profileScaled)
    });
}

int Z2::setProfileRGBA(unsigned char profile, unsigned char red, unsigned char green, unsigned char blue) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    unsigned char verification = 0x55 - (red + green + blue);
    std::vector<unsigned char> data = {
        0x08, WriteFlashData, 0x00, 0x00, (unsigned char)(0x2C + (profile * 4)), 0x04, red, green, blue, verification, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(0xC1 - (profile * 4))
    };

    return sendPacket(data);
}


int Z2::setDPILightEffect(DPILightEffect lightEffect) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x52, 0x02, (unsigned char)lightEffect, (unsigned char)(0x55 - lightEffect), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9D
    });
}

int Z2::setDPILightBrightness(unsigned char brightness) {
    if(brightness < 1) return -2;
    if(brightness > 10) return -3;
    
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    // TODO: should figure this out a better to do this
    std::map<unsigned char, std::pair<unsigned char, unsigned char>> brightnessMap = {
        { 1, std::pair<unsigned char, unsigned char>{0x10, 0x45} },
        { 2, std::pair<unsigned char, unsigned char>{0x1E, 0x37} },
        { 3, std::pair<unsigned char, unsigned char>{0x3C, 0x19} },
        { 4, std::pair<unsigned char, unsigned char>{0x5A, 0xFB} },
        { 5, std::pair<unsigned char, unsigned char>{0x80, 0xD5} },
        { 6, std::pair<unsigned char, unsigned char>{0x96, 0xBF} },
        { 7, std::pair<unsigned char, unsigned char>{0xB4, 0xA1} },
        { 8, std::pair<unsigned char, unsigned char>{0xD2, 0x83} },
        { 9, std::pair<unsigned char, unsigned char>{0xE6, 0x6F} },
        { 10, std::pair<unsigned char, unsigned char>{0xFF, 0x56} }
    };

    auto pair = brightnessMap[brightness];
    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x4E, 0x02, pair.first, pair.second, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xA1
    });
}

int Z2::setDPILightFlickerSpeed(unsigned char speed) {
    if(speed < 1) return -2;
    if(speed > 5) return -3;
    
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x50, 0x02, speed, (unsigned char)(0x55 - speed), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9F
    });
}


int Z2::setMotionSync(bool enabled) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0xAB, 0x02, enabled, (unsigned char)(0x55 - enabled), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x44
    });
}

int Z2::setAngleSnapping(bool enabled) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0xAF, 0x02, enabled, (unsigned char)(0x55 - enabled), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x40
    });
}

int Z2::setRippleControl(bool enabled) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0xB1, 0x02, enabled, (unsigned char)(0x55 - enabled), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x3E
    });
}


int Z2::setSensorLOD(unsigned char mm) {
    if(mm < 1) return -2;
    else if(mm > 2) return -3;

    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x0A, 0x02, mm, (unsigned char)(0x55 - mm), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xE5
    });
}

int Z2::setSensorHighPower(bool enabled) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0xB9, 0x02, enabled, (unsigned char)(0x55 - enabled), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x36
    });
}

int Z2::setReportRate(ReportRate rate) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    if(isWired) {
        switch (rate) {
        case ReportRate::R_2000: case ReportRate::R_4000: return -2;
        default: break;
        }
    }

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x00, 0x02, (unsigned char)rate, (unsigned char)(0x55 - (unsigned char)rate), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xEF
    });
}


int Z2::setLongDistance(bool enabled) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;
    
    ret = sendPacket({
        0x08, SetLongRangeMode, 0x00, 0x00, 0x00, 0x0A, enabled, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(0x2C + enabled)
    });

    return ret;
}

int Z2::setPeakPerformance(bool enabled) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;
    
    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0xB5, 0x02, enabled, (unsigned char)(0x55 - enabled), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x3A
    });
}

int Z2::setPeakPerformanceTimer(TimerDurations duration) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    std::map<TimerDurations, unsigned char> timerMap = {
        { SECONDS_30, 0x52 },
        { MINUTES_1, 0x4F },
        { MINUTES_5, 0x37 },
        { MINUTES_10, 0x19 },
        { MINUTES_15, 0xFB },
        { MINUTES_20, 0xDD },
        { MINUTES_25, 0xBF },
        { MINUTES_30, 0xA1 },
        { MINUTES_35, 0x83 },
        { MINUTES_40, 0x65 }
    };

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0xB7, 0x02, (unsigned char)duration, timerMap[duration], 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x38
    });
}


int Z2::getProfile() {
    auto data = getFlashData(0x04);
    if(data.size() <= 0) return -1;
    
    return data[0];
}

int Z2::getProfileCount() {
    auto data = getFlashData(0x02);
    if(data.size() <= 0) return -1;
    
    return data[0];
}

Z2::ReportRate Z2::getReportRate() {
    auto data = getFlashData(0x00);
    if(data.size() <= 0) return Z2::ReportRate::R_INVALID;
    
    return (ReportRate)data[0];
}


// generated with chatgpt lol
int Z2::getBatteryPercentage() {
    uint16_t combinedVoltage = getBatteryCharge();
    
    // Voltage thresholds and corresponding percentages
    uint16_t voltageThresholds[] = { 
        0x0C7A, 0x0D53, 0x0D6C, 0x0D9A, 0x0DE1, 0x0E28, 0x0E39, 0x0E5F, 
        0x0EB6, 0x0F12, 0x0F1F, 0x0F34, 0x0F45, 0x0F5E, 0x0FD3, 0x0FD7, 
        0x0FDF, 0x0FE3, 0x0FE8, 0x0FEC, 0x1072
    };
    int percentageThresholds[] = { 
        0, 4, 6, 10, 16, 21, 23, 26, 
        35, 46, 48, 51, 53, 57, 87, 
        88, 90, 92, 93, 94, 100 
    };

    // Determine battery percentage using linear interpolation
    int batteryPercentage = 0;
    size_t i;

    // Find the appropriate range for interpolation
    for (i = 1; i < sizeof(voltageThresholds) / sizeof(voltageThresholds[0]); ++i) {
        if (combinedVoltage <= voltageThresholds[i]) {
            // Linear interpolation between percentageThresholds[i-1] and percentageThresholds[i]
            int lowerPercentage = percentageThresholds[i - 1];
            int upperPercentage = percentageThresholds[i];
            uint16_t lowerVoltage = voltageThresholds[i - 1];
            uint16_t upperVoltage = voltageThresholds[i];
            
            // Calculate interpolated battery percentage
            batteryPercentage = lowerPercentage + 
                                ((upperPercentage - lowerPercentage) * 
                                (combinedVoltage - lowerVoltage)) / 
                                (upperVoltage - lowerVoltage);
            break;
        }
    }
    
    // Edge case for very low voltages
    if (i == sizeof(voltageThresholds) / sizeof(voltageThresholds[0])) {
        batteryPercentage = percentageThresholds[sizeof(voltageThresholds) / sizeof(voltageThresholds[0]) - 1];
    }
    
    return batteryPercentage;
}


short Z2::getBatteryCharge() {
    std::vector<unsigned char> buf(17);
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    hid_read(device, buf.data(), buf.size());

    ret = sendPacket({
        0x08, BatteryLevel, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x49
    });

    if(ret < 0) {
        return ret;
    }

    hid_read(device, buf.data(), buf.size());
    return (short)(buf[8] << 8) + (short)buf[9];
}


Z2* findZ2() {
    hid_device_info* devices = hid_enumerate(0x3554, 0);
    
    if(devices) {
        unsigned int PID = devices->product_id;
        hid_free_enumeration(devices);

        return new Z2(PID);
    }

    return nullptr;
}