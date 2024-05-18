#include "./Z2.hpp"

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

    unsigned char firstByte = std::ceil(DPI / 50.0f) - 1;
    unsigned char secondByte = 0x55 - (firstByte * 2);
    unsigned char overflowedByte = 0x00;

    // TODO: theres probably a better way to do this
    if(DPI > 25600) {
        overflowedByte = 0x88;

        secondByte = 0xCD - (firstByte * 2);
    }
    else if(DPI > 12800) {
        overflowedByte = 0x44;

        secondByte = 0x11 - (firstByte * 2);
    }

    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, (unsigned char)(0x0C + (profile * 4)), 0x04, firstByte, firstByte, overflowedByte, secondByte, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(0xE1 - (profile * 4))
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

int Z2::setReportRate(unsigned int rate) {
    unsigned int max = std::numeric_limits<unsigned int>::max();
    if(rate >= max) return -2;

    auto it = std::find(rates.begin(), rates.end(), rate);
    if(it == rates.end()) {
        return -2;
    }

    ptrdiff_t index = std::distance(rates.begin(), it);

    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    return sendPacket({
        0x08, WriteFlashData, 0x00, 0x00, 0x00, 0x02, (unsigned char)index, (unsigned char)(0x55 - (unsigned char)index), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xEF
    });
}

int Z2::setLongDistance(bool enabled) {
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;
    
    if(enabled) {
        ret = sendPacket({
            0x08, SetLongRangeMode, 0x00, 0x00, 0x00, 0x0A, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x2C
        });
    }
    else {
        ret = sendPacket({
            0x08, SetLongRangeMode, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x2D
        });
    }

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


char Z2::getBatteryPercentage() {
    std::vector<unsigned char> buf(17);
    int ret = sendPacket(initialPacket);
    if(ret < 0) return ret;

    hid_read(device, buf.data(), buf.size());

    // TODO: 100 for some reason on zaopin software must find why
    //  08 04 00 00 00 02 5F 00 10 1A 00 00 00 00 00 00
    //  BE

    ret = sendPacket({
        0x08, BatteryLevel, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x49
    });

    if(ret < 0) {
        return ret;
    }

    hid_read(device, buf.data(), buf.size());

    return (int)buf[6];
}

short Z2::getBatteryVoltage() {
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
    return (short)(buf[8] << 8) + buf[9];
}