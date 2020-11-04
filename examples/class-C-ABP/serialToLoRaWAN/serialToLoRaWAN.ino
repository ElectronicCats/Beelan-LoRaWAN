#include <lorawan.h>

//ABP Credentials 
const char *devAddr = "01addd38";
const char *nwkSKey = "b2ef58d25e9523ab8618c5e6ce1d1b66";
const char *appSKey = "02e06f9ccccd4f0a4376a89954be606c";

char myStr[240];
char outStr[255];
byte recvStatus = 0;

const sRFM_pins RFM_pins = {
  .CS = SS,
  .RST = RFM_RST,
  .DIO0 = RFM_DIO0,
  .DIO1 = RFM_DIO1,
  .DIO2 = RFM_DIO2,
  .DIO5 = RFM_DIO5,
};


void setup() {
  // Setup loraid access
  Serial.begin(115200);
  Serial.println("Start..");
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_C);

  // Set Data Rate
  lora.setDataRate(SF7BW125);

  // set channel to random
  lora.setChannel(MULTI);
  
  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

void loop() {

  if(Serial.available()){
    uint8_t i = 0;
    while(Serial.available()>0){
      myStr[i++]=Serial.read();
      if(i>240){
        Serial.println("[ERROR] payload maximo alcanzado");
        break;
        }
      }
    Serial.print("Uplink: ");
    Serial.println(myStr);
    lora.sendUplink(myStr, strlen(myStr), 0, 1);
  }

  recvStatus = lora.readData(outStr);
  if(recvStatus) {
    Serial.print("Downlink: ");
    Serial.println(outStr);
  }
  
  // Check Lora RX
  lora.update();
}
