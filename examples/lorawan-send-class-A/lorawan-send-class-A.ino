#include <lorawan.h>

long interval = 10000;    // 10 s interval to send message
long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

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
  if(!lora.init()){
  Serial.println("RFM95 not detected");
  while(1);
  }

  // Set LoRaWAN Class
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(3);
  
  // Put ABP Key and DevAddress here
  lora.setNwkSKey("b7300d9f68b649ed30530f9dd69f9afe");
  lora.setAppSKey("9d52eef7fab63eda18794d0e503ddf20");
  lora.setDevAddr("07000007");
}

void loop() {
  // put your main code here, to run repeatedly:
  char myStr[50];
  char outStr[255];
  int recvStatus = 0;
  
  unsigned long currentMillis = millis();

  // Check interval overflow
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 

    sprintf(myStr, "Ini data LoRa ke-%d", counter); 
    lora.sendUplink((unsigned char *)myStr, strlen(myStr), 0);
    counter++;
  }

  recvStatus = lora.readData(outStr);
  if(recvStatus) {
    Serial.println(outStr);
  }
  
  // Check Lora RX
  lora.update();
}