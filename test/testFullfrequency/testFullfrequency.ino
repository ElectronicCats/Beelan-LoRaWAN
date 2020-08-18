/**
 *  Test in all frequencies of US_915 band
 *      Using SUB BAND 1 [ 903.9 - 905.3 Mhz] see config.h
 *      channel [0 - 7]
 *      data rate [0 - 3]
 *  Class A and C devices with ABP activation
 *      
 *  Gateway:    Sentrius RG101
 * 
 * Using CATWAN USB STICK hardware
 * 
 * Author: Ivan Moreno
 *  May 2019
 */

#include <lorawan.h>

// ABP device
const char *devAddr = "00000000";
const char *nwkSKey = "00000000000000000000000000000000";
const char *appSKey = "00000000000000000000000000000000";


unsigned long interval = 15000;   // 15 s interval to send message
unsigned long previousMillis = 0; // will store last time message sent
unsigned long currentMillis;

union _cntCI {
  unsigned char _cb[2];
  unsigned int val;
} counter;

char payload[10];
char outStr[100];
byte recvStatus = 0;

byte _dataRate;
byte _channel;
byte _times;

const sRFM_pins RFM_pins = {
  .CS = 20,
  .RST = 9,
  .DIO0 = 0,
  .DIO1 = 1,
  .DIO2 = 2,
  .DIO5 = 15,
};

void setup() {
  // seti serial port
  Serial.begin(115200);
  delay(4000);

  if(!lora.init()){
    Serial.println("RFM95 not detected");
    return;
  }

  // Set LoRaWAN Class
  lora.setDeviceClass(CLASS_A);

  lora.setTxPower(15,PA_BOOST_PIN);

  counter.val = 0;
  _channel = 0; // 0 - 7
  _dataRate = 0; // 0 - 3 
  _times = 0;

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

void loop() {
  currentMillis = millis();
  // Check interval overflow
  if(currentMillis - previousMillis > interval) {
    if(_times > 0){
        _times = 0;
        _dataRate++;
        if (_dataRate > 3){
            _dataRate = 0;
            _channel++;
            if (_channel > 7) {
                _channel = 0;
            } 
        }
    }
    // Set Channel
    lora.setChannel(_channel);

    // Set Data Rate
    lora.setDataRate(_dataRate);
    
    Serial.print("SND[");
    Serial.print(counter.val);
    Serial.print("], CH: ");
    Serial.print(_channel);    
    Serial.print(", DR: ");
    Serial.println(_dataRate);

    previousMillis = currentMillis; 

    payload[0] = _channel;
    payload[1] = _dataRate;
    payload[2] = 15;
    payload[3] = _times + 1;
    payload[4] = counter._cb[1];
    payload[5] = counter._cb[0];

    lora.sendUplink(payload, 6, 0, 1);
    counter.val++;
    _times++;
  }

  // Check Lora RX
  lora.update();

  recvStatus = lora.readData(outStr);
  if(recvStatus) {
    Serial.print("Rcv: ");
    Serial.println(outStr);
  }
}
