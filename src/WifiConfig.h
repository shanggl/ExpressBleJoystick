#ifndef WIFI_CONFIG
#define WIFI_CONFIG

 
class WifiConfig{


public:
    WifiConfig(void){
     
    }

    void init_web();
    static void begin();
    static void end();
    static void handleWifiEvent(void* );

private:


};


#endif