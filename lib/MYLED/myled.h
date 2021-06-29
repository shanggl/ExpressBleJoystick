#pragma once
#include "targets.h"

class myled
{
private:
    static int ledPin;
    static bool activeHigh;
    static bool stateOn ;


public:
    static void init(int Pin);
    static void blink();
    static void on();
    static void off();
    static bool inline isOn(){return stateOn;};
};