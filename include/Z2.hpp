#ifndef __Z2_HPP__
#define __Z2_HPP__

#include "./device.hpp"

#include <set>
#include <map>
#include <cmath>

class Z2 : public Device {
private:
    enum UsbCommandID {
		EncryptionData = 1,
		PCDriverStatus,
		DeviceOnLine,
		BatteryLevel,
		DongleEnterPair,
		GetPairState,
		WriteFlashData,
		ReadFlashData,
		ClearSetting,
		StatusChanged,
		SetDeviceVidPid,
		SetDeviceDescriptorString,
		EnterUsbUpdateMode,
		GetCurrentConfig,
		SetCurrentConfig,
		ReadCIDMID,
		EnterMTKMode,
		ReadVersionID,
		Set4KDongleRGB = 20,
		Get4KDongleRGBValue,
		SetLongRangeMode,
		GetLongRangeMode,
		WriteKBCIdMID = 240,
		ReadKBCIdMID
	};

    // packet layout (packets are 17 bytes)
    // Report ID | Mode | Reserved | Type | DataLen |        Data + Reserved        |  Type Inverted (unsure) | Result
    // 08          03     00    00   00     00        00 00 00 00 00 00 00 00 00 00    00                       Unsure
    // 08          04     00    00   00     00        00 00 00 00 00 00 00 00 00 00    49                       Get Battery
    // 08          07     00    00   04     02        00 55 00 00 00 00 00 00 00 00    EB                       Sets Current DPI Profile to 0
    // 08          07     00    00   04     02        01 54 00 00 00 00 00 00 00 00    EB                       Sets Current DPI Profile to 1
    
    const std::vector<unsigned char> initialPacket = {
        0x08, DeviceOnLine, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x4A
    };


    const unsigned int RESERVED = std::numeric_limits<unsigned int>::max();
    const std::vector<unsigned int> rates = {
        RESERVED, 1000,
        500,
        RESERVED, 250,
        RESERVED, RESERVED, RESERVED, 125,
        RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, 2000,
        RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, RESERVED, 4000
    };

public:    
    enum TimerDurations {
        SECONDS_30 = 0x03,
        MINUTES_1 = 0x06,
        MINUTES_5 = 0x1e,
        MINUTES_10 = 0x3C,
        MINUTES_15 = 0x5A,
        MINUTES_20 = 0x78,
        MINUTES_25 = 0x96,
        MINUTES_30 = 0xB4,
        MINUTES_35 = 0xD2,
        MINUTES_40 = 0xF0
    };

    enum DPILightEffect {
        OFF,
        FIXED,
        FLICKER
    };

    // wired is PID 0xF526
    // Z2() : Device(0x3554, 0xF526, 0xff02, 0x0002, []() -> void {}) {
    Z2() : Device(0x3554, 0xF527, 0xff02, 0x0002, []() -> void {}) {}

    int setDPIProfile(unsigned char profile);
    int setDPIProfilesCount(unsigned char count);
    // (rounds to nearest 50)
    int setProfileDPI(unsigned char profile, unsigned int DPI);
    int setProfileRGBA(unsigned char profile, unsigned char red, unsigned char green, unsigned char blue);

    int setDPILightEffect(DPILightEffect lightEffect);    
    // only works when on fixed
    int setDPILightBrightness(unsigned char brightness);
    // only works when on flicker
    int setDPILightFlickerSpeed(unsigned char speed);

    int setMotionSync(bool enabled);
    int setAngleSnapping(bool enabled);
    int setRippleControl(bool enabled);

    int setSensorLOD(unsigned char mm);
    int setSensorHighPower(bool enabled);
    int setReportRate(unsigned int rate);

    int setLongDistance(bool enabled);

    int setPeakPerformance(bool enabled);
    int setPeakPerformanceTimer(TimerDurations duration);

    char getBatteryPercentage();
    short getBatteryVoltage();
};

#endif