
#include "targets.h"
#include "myled.h"
#include "button.h"
#include "WifiConfig.h"





WifiConfig wifiConfig;
int count;
myled led;
button button;

void buttonLongDown(){
  if(led.isOn()){
    Serial.println("button long pressed trun off led");
    led.off();
  }else{
    Serial.println("button long pressed trun on led");
    led.on();
  }
}

void buttonShortDown(){
  Serial.println("button short pressed blink led");

  led.blink();
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(460800);
  count=0;

  wifiConfig.init_web();
  wifiConfig.begin();  
  Serial.println("begin:setup ===================== ");

  led.init(GPIO_PIN_LED);
  button.init(GPIO_BUTTON_BOOT_PIN,true);
  button.buttonShortPress=&buttonShortDown;
  button.buttonLongPress=&buttonLongDown;
  
}

void loop() {
 // put your main code here, to run repeatedly:
  //Serial.printf("in loop ~:tiko tiko [%d]\n",count);
  // count++;
  //just for test 


  button.handle();

}

