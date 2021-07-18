
#include "targets.h"
#include "myled.h"
#include "ESP32_BLE_HID.h"
#include "hwTimer.h"
#include "common.h"
//esp32 on board led 2
myled led;
bool BLEjoystickActive = false;
volatile bool BLEjoystickRefresh = false;
bool UpdateParamReq =false; 

// for lua 
const uint8_t thisCommit[6] = {1,2,3,3,4,5};
uint8_t luaCommitPacket[7] = {(uint8_t)0xFE, thisCommit[0], thisCommit[1], thisCommit[2], thisCommit[3], thisCommit[4], thisCommit[5]};

//// CONSTANTS ////
#define RX_CONNECTION_LOST_TIMEOUT 3000LU // After 3000ms of no TLM response consider that slave has lost connection
#define MSP_PACKET_SEND_INTERVAL 10LU

#ifndef TLM_REPORT_INTERVAL_MS
#define TLM_REPORT_INTERVAL_MS 320LU // Default to 320ms
#endif

volatile uint32_t LastTLMpacketRecvMillis = 0;
uint32_t TLMpacketReported = 0;


hwTimer hwTimer;
GENERIC_CRC14 ota_crc(ELRS_CRC14_POLY);
CRSF crsf;
MSP msp;

bool busyTransmitting=false;
long LuaLastUpdated=0;

//call back mark data need send to ble 
void ICACHE_RAM_ATTR SendRCdataToBLE()
{
  BLEjoystickRefresh = true;
  led.blink();
}

void ICACHE_RAM_ATTR ParamUpdateReq()
{
  Serial.println("update params recived from lua");
  UpdateParamReq = true;
  led.blink();
}



//lua 脚本读取当前配置，备用
void sendLuaParams()
{
/*
 (uint8_t)(InBindingMode | (webUpdateMode << 1)),
  (uint8_t)ExpressLRS_currAirRate_Modparams->enum_rate,
  (uint8_t)(ExpressLRS_currAirRate_Modparams->TLMinterval),
  (uint8_t)(POWERMGNT.currPower()),
  (uint8_t)Regulatory_Domain_Index,
  (uint8_t)crsf.BadPktsCountResult,
  (uint8_t)((crsf.GoodPktsCountResult & 0xFF00) >> 8),
  (uint8_t)(crsf.GoodPktsCountResult & 0xFF),
  (uint8_t)LUA_VERSION};
*/
  Serial.println("begin send lua pong response ");
  uint8_t luaParams[] = {0xFF,
                         (uint8_t)(0x00),//webupdate|bind mode
                         (uint8_t)0x01,//rate 
                         (uint8_t)0x00,//TLMinterval 
                         (uint8_t)0x04,//power 
                         (uint8_t)0x06,//Regulatory_Domain_Index
                         (uint8_t)crsf.BadPktsCountResult,
                         (uint8_t)((crsf.GoodPktsCountResult & 0xFF00) >> 8),
                         (uint8_t)(crsf.GoodPktsCountResult & 0xFF),
                         (uint8_t) LUA_VERSION };

  crsf.sendLUAresponse(luaParams, 10);//data ,len 
}

//处理发来的msp 包
void ProcessMSPPacket(mspPacket_t *packet)
{
  // Inspect packet for ELRS specific opcodes
  if (packet->function == MSP_ELRS_FUNC)
  {
    uint8_t opcode = packet->readByte();

    CHECK_PACKET_PARSING();

    switch (opcode)
    {
    case MSP_ELRS_RF_MODE:
      // OnRFModePacket(packet);
      Serial.println("on RF MODe packet");
      break;
    case MSP_ELRS_TX_PWR:
      Serial.println("on TXPOWER packet");

      // OnTxPowerPacket(packet);
      break;
    case MSP_ELRS_TLM_RATE:
      // OnTLMRatePacket(packet);
      Serial.println("on TLMtrate Mode packet");
      break;
    default:
      break;
    }
  }
  else if (packet->function == MSP_SET_VTX_CONFIG)
  {
    crsf.AddMspMessage(packet);
  }
}
//处理 lua msp 请求
void HandleUpdateParameter()
{
  if (millis() > LuaLastUpdated + OPENTX_LUA_UPDATE_INTERVAL)
  {
    sendLuaParams();
    LuaLastUpdated = millis();
  }

  if (UpdateParamReq == false)
  {
    return;
  }

  switch (crsf.ParameterUpdateData[0])
  {
  case 0: // special case for sending commit packet
    Serial.println("send lua commit packet \n");
    crsf.sendLUAresponse(luaCommitPacket, 7);
    break;

  case 1:
    {
      Serial.print("Request AirRate: ");
      Serial.println(crsf.ParameterUpdateData[1]);
    }
    break;

  case 2:
    {
      Serial.print("Request TLM interval: ");
      Serial.println(crsf.ParameterUpdateData[1]);
    }
    break;

  case 3:
    Serial.print("Request Power: ");
    Serial.println(crsf.ParameterUpdateData[1]);
    break;

  case 4:
    break;

  case 0xFE:// 启动蓝牙
    if (crsf.ParameterUpdateData[1] == 1)
    {
      Serial.println("ble request from lua");
      BluetoothJoystickBegin();
      hwTimer.callbackTock = &SendRCdataToBLE;
      crsf.setSyncParams(8000); // 125hz
      hwTimer.updateInterval(8000);
      crsf.RCdataCallback = &BluetoothJoystickUpdateValues;
      BLEjoystickActive = true; 
    }
    break;

  case 0xFF:
    if (crsf.ParameterUpdateData[1] == 1)
    {
      Serial.println("Binding requested from LUA");
    }
    else
    {
      Serial.println("Binding stopped  from LUA");
    }
    break;

  default:
    break;
  }
  UpdateParamReq = false;
}


void UARTdisconnected()
{
  hwTimer.stop();
  led.off();
}

void UARTconnected()
{

  //inital state variables, maybe move elsewhere?
  for (int i = 0; i < 2; i++) // sometimes OpenTX ignores our packets (not sure why yet...)
  {
    crsf.sendLUAresponse(luaCommitPacket, 7);
    delay(100);
    sendLuaParams();
    delay(100);
  }
  hwTimer.resume();
  led.on();
}

void ICACHE_RAM_ATTR ProcessTLMpacket()
{//todo 根据蓝牙链接情况， 设置链接

  if (connectionState != connected)
  {
    connectionState = connected;
    // LPD_DownlinkLQ.init(100);
    // Serial.println("got downlink conn");
  }

  LastTLMpacketRecvMillis = millis();

  // LQCalc.add();
//do fake link statics
   {
      // RSSI received is signed, proper polarity (negative value = -dBm)
        crsf.LinkStatistics.uplink_RSSI_1 = 60;
        crsf.LinkStatistics.uplink_RSSI_2 = 60;
        crsf.LinkStatistics.uplink_SNR = 100;
        crsf.LinkStatistics.uplink_Link_quality = 100;
        crsf.LinkStatistics.uplink_TX_Power = 3 ;
        crsf.LinkStatistics.downlink_SNR = 100;
        crsf.LinkStatistics.downlink_RSSI = 60;
        crsf.LinkStatistics.downlink_Link_quality =100; // +1 fixes rounding issues with filter and makes it consistent with RX LQ Calculation
        crsf.LinkStatistics.rf_Mode = 3;

    }

}

void ICACHE_RAM_ATTR RXdoneISR()
{
  ProcessTLMpacket();
  busyTransmitting = false;
}

void ICACHE_RAM_ATTR TXdoneISR()
{
  busyTransmitting = false;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(460800);
  Serial.println("begin:setup ===================== ");
//led on board
  led.init(GPIO_PIN_LED);
 
//init ble joy stick
  BluetoothJoystickBegin();

  crsf.connected = &UARTconnected; // it will auto init when it detects UART connection
  crsf.disconnected = &UARTdisconnected;
  crsf.RecvParameterUpdate = &ParamUpdateReq;
  crsf.Begin();
  connectionState = connected;
}

void loop() {
 // put your main code here, to run repeatedly:
  uint32_t now = millis();

  if (BLEjoystickActive && BLEjoystickRefresh)
    {
      // HandleUpdateParameter();
      BluetoothJoystickSendReport();
      BLEjoystickRefresh = false;
     }
  if (UpdateParamReq){
    HandleUpdateParameter();
  }


// process ble connect or not 
// to do ,read ble status ;
  // if (now > (RX_CONNECTION_LOST_TIMEOUT + LastTLMpacketRecvMillis))
  // {
  //   connectionState = disconnected;
  // }
  // else
  // {
  //   connectionState = connected;
  
  // } 

  if (Serial.available())
  {
    uint8_t c = Serial.read();

    if (msp.processReceivedByte(c))
    {
      // Finished processing a complete packet
      ProcessMSPPacket(msp.getReceivedPacket());
      msp.markPacketReceived();
    }
  }

  /* Send TLM updates to handset if connected + reporting period
   * is elapsed. This keeps handset happy dispite of the telemetry ratio */
  if ((connectionState == connected) && (LastTLMpacketRecvMillis != 0) &&
      (now >= (uint32_t)(TLM_REPORT_INTERVAL_MS + TLMpacketReported))) {
    Serial.println("sent link statics to Tx");
    crsf.sendLinkStatisticsToTX();
    TLMpacketReported = now;
  }

  // send tlm back to tx 
  RXdoneISR();


}

