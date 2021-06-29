#include "WifiConfig.h"
#include "esp32-hal-uart.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>

TaskHandle_t xWebConfigEventTask = NULL;
SemaphoreHandle_t mutexOutFIFO = NULL;
static bool wifi_config_enabled ;

void WifiConfig :: init_web()
{
    wifi_config_enabled= false;
    Serial.println("init wificonfig ...");
}

void WifiConfig ::begin(){

    if (wifi_config_enabled)
    {
        return;
    }
    else{
        //create the task , pring doing every 1s
        Serial.println("About to start wifi task...");
        mutexOutFIFO = xSemaphoreCreateMutex();
        disableCore0WDT();
        xTaskCreatePinnedToCore(handleWifiEvent, "xWebConfigEventTask", 3000, NULL, 0, &xWebConfigEventTask, 0);
        wifi_config_enabled =true;
    }
 }

void WifiConfig::handleWifiEvent(void *){
    // 
    int count=1;
while(1){
    Serial.printf("tiko tiko [%d]\n",count*10);
    vTaskDelay(count*10/portTICK_RATE_MS);
    count++; 
    }

}

void WifiConfig:: end(){
    if (wifi_config_enabled == false)
    {
        /* code */
        Serial.println("wifi config end");
        return;
     }

    if (xWebConfigEventTask != NULL)
    {
        vTaskDelete(xWebConfigEventTask);
    }
 
    wifi_config_enabled= false;

     
}
