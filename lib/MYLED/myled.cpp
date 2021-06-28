#include "myled.h"

int myled::ledPin=-1;
bool myled::activeHigh=false;
bool myled::stateOn=false;

void myled::init (int Pin,bool ActiveHigh){
    pinMode(Pin,OUTPUT);
    ledPin = Pin;
    activeHigh = ActiveHigh; 
    stateOn=false;   
}


void myled::on(){
    digitalWrite(ledPin,HIGH);//0 for low
    stateOn=true;
}

void myled::off(){
    digitalWrite(ledPin,LOW);//1 for high
    stateOn=false;
}

void myled::blink(){
    for(int i=0;i<3;i++)
    {
        on();
        vTaskDelay(500);
        off();
    }
}