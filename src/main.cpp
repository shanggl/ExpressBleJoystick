
#include "targets.h"
#include "common.h"
#include "myled.h"
#include "button.h"

#include <Wire.h>
#include <SSD1306.h>
#include "ESP32_BLE_HID.h"




int count;
myled led;
button button;
float buttonValue;

SSD1306 display(0x3c, GPIO_PIN_OLED_SDA, GPIO_PIN_OLED_SCL);

void buttonLongDown(){
  if(led.isOn()){
    Serial.println("button long pressed trun off led");
    display.clear();
    display.drawString(0, 0, "button long pressed trun off led");
    display.display();
  
    led.off();
  }else{
    Serial.println("button long pressed trun on led");
    display.clear();
    display.drawString(0,0,"button long pressed trun on led");
    display.display();
    led.on();
  }
}

void buttonShortDown(){
  Serial.println("button short pressed blink led");
    led.blink();
    display.clear();
    display.drawString(0,0,"blink led");
    display.display();
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(460800);
  Serial.println("begin:setup ===================== ");
//led on board
  led.init(GPIO_PIN_LED);
//'boot' button on board 
  button.init(GPIO_BUTTON_BOOT_PIN,true);
  button.buttonShortPress=&buttonShortDown;
  button.buttonLongPress=&buttonLongDown;
//i2c ssd1306 12864 display 
  display.init();
  //重置屏幕方向
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  display.drawString(0, 0, "BLE_Joystick init!");
  display.display();

}

void loop() {
 // put your main code here, to run repeatedly:

  button.handle();
 
 

  // rebuild the picture after some delay


}

