#ifndef __Z2_HPP__
#define __Z2_HPP__

#include "./device.hpp"

#include <set>
#include <map>
#include <cmath>

class Z2 : public Device {
private:
    bool isWired;

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

    std::vector<unsigned char> getFlashData(unsigned char commandID);
public:
    enum ConnectType {
        WIRED,
        WIFI
    };

    static unsigned int getPID(ConnectType connectType) {
        switch(connectType) {
        case ConnectType::WIRED: return 0xF526;
        default: return 0xF527;
        }
    }


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

    enum ReportRate {
        R_INVALID = -1,
        R_1000 = 1,
		R_500 = 2,
		R_250 = 4,
		R_125 = 8,
		R_2000 = 16,
		R_4000 = 32
    };

    // wired is PID 0xF526 wireless(2.4GHz) is PID 0xF527 not sure about bluetooth
    Z2(ConnectType connectType) : Device(0x3554, getPID(connectType), 0xff02, 0x0002, []() -> void {}) {
        switch (connectType) {
        case ConnectType::WIFI:
            isWired = false;
            break;
        default: // assuming bluetooth will be 1khz max
            isWired = true;
            break;
        }
    }

    Z2(unsigned int PID) : Device(0x3554, PID, 0xff02, 0x0002, []() -> void {}) {
        switch (PID) {
        case 0xF526:
            isWired = false;
            break;
        default: // assuming bluetooth will be 1khz max
            isWired = true;
            break;
        }
    }

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
    int setReportRate(ReportRate rate);

    int setLongDistance(bool enabled);

    int setPeakPerformance(bool enabled);
    int setPeakPerformanceTimer(TimerDurations duration);


    int getProfile();
    int getProfileCount();
    ReportRate getReportRate();

    int getBatteryPercentage();
    short getBatteryCharge();
};


// no way to pick between multiple
Z2* findZ2();

#endif