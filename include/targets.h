#pragma once
#if !defined TARGET_NATIVE
#include <Arduino.h>
#endif
// for esp32 devkit esp-wroom32-s
#define GPIO_BUTTON_BOOT_PIN  0
#define GPIO_BUTTON_5D1WIRE_PIN 4
#define GPIO_PIN_LED 2

#define GPIO_PIN_OLED_SCL 23
#define GPIO_PIN_OLED_SDA 22

#define LUA_VERSION 3
#define OPENTX_LUA_UPDATE_INTERVAL 100


 



//make shure give the right pin  13
#define GPIO_PIN_RCSIGNAL_RX 13
#ifdef USE_TTL_COM
    #define GPIO_PIN_RCSIGNAL_TX 13
#else
    #define GPIO_PIN_RCSIGNAL_TX 12
#endif