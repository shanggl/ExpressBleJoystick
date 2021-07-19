#pragma once

#if defined(PLATFORM_ESP32) && defined(BLE_HID_JOYSTICK)

#include <Arduino.h>
#include "targets.h"
#include <BleGamepad.h>

#include "CRSF.h"
extern CRSF crsf;

#define numOfButtons 0
#define numOfHatSwitches 0
#define enableX true
#define enableY true
#define enableZ false
#define enableRZ false
#define enableRX true
#define enableRY true
#define enableSlider1 true
#define enableSlider2 true
#define enableRudder false
#define enableThrottle false
#define enableAccelerator false
#define enableBrake false
#define enableSteering false


//蓝牙名称，厂商，电量
BleGamepad bleGamepad("ExpressLRS-Joystick", "EMSR", 100);

void BluetoothJoystickBegin()
{
    bleGamepad.setAutoReport(false);
    Serial.println("Starting BLE Joystick!");
    bleGamepad.setControllerType(CONTROLLER_TYPE_MULTI_AXIS);
    bleGamepad.begin(numOfButtons, numOfHatSwitches, enableX, enableY, enableZ, enableRZ, enableRX, enableRY, enableSlider1, enableSlider2, enableRudder, enableThrottle, enableAccelerator, enableBrake, enableSteering);
    bleGamepad.setAutoReport(false);   //This is true by default
}

void BluetoothJoystickSendReport()
{
    if (bleGamepad.isConnected())
    {
        bleGamepad.sendReport();
    }
}

void BluetoothJoystickUpdateValues()
{
    if (bleGamepad.isConnected())
    {
        // Serial.println("crsf update ble values");
        int16_t data[sizeof(crsf.ChannelDataIn)] = {0};

        for (uint8_t i = 0; i < 9; i++)
        {
            data[i] = map(crsf.ChannelDataIn[i], CRSF_CHANNEL_VALUE_MIN - 1, CRSF_CHANNEL_VALUE_MAX + 1, -32768, 32768);
        }

        bleGamepad.setX(data[0]);
        bleGamepad.setY(-data[1]);//不知道为啥，反正要反过来
        bleGamepad.setRX(data[2]);
        bleGamepad.setRY(data[3]);
        bleGamepad.setSlider1(data[4]);
        bleGamepad.setSlider2(data[5]);
        bleGamepad.setZ(data[6]);
        bleGamepad.setRZ(data[7]);
    }
    // process 
}

#endif