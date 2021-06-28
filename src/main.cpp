#include <Arduino.h>
#include "WifiConfig.h"
#include "target.h"
#include "button.h"
#include <driver/gpio.h>



WifiConfig wifiConfig;
int count;



void setup() {
  // put your setup code here, to run once:

  Serial.begin(460800);
  count=0;

  wifiConfig.init_web();
  wifiConfig.begin();  
  Serial.println("begin:setup ===================== ");
}

void loop() {
 // put your main code here, to run repeatedly:
  Serial.printf("in loop ~:tiko tiko [%d]\n",count);
   count++;
  delay(1000);
  if(count >50){
    wifiConfig.end();
  }
}



