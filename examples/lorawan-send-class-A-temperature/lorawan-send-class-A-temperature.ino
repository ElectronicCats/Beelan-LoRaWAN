#include <lorawan.h>
#include <TemperatureZero.h>
TemperatureZero TempZero = TemperatureZero();

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
  while(!Serial);
  if(!lora.init()){
  Serial.println("RFM95 not detected");
  while(1);
  }

  // Set LoRaWAN Class
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(3);
  
  // Put ABP Key and DevAddress here
  lora.setNwkSKey("DB73593681060DF7EBE7A13B3964D129");
  lora.setAppSKey("9720AED7F935862FF794F7926BBE1E7C");
  lora.setDevAddr("260217D9");
}

void loop() {
  // put your main code here, to run repeatedly:
  char myStr[50];
  uint8_t mydata[10];
  char outStr[255];
  int recvStatus = 0;
  
  unsigned long currentMillis = millis();

  // Check interval overflow
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 

    float temp =   TempZero.readInternalTemperature();
    Serial.print(F("[INFO] Temperature:")); SerialUSB.println(temp);
    int val = round(temp * 10);
    mydata[0] = 0;
    mydata[1] = 0x67;
    mydata[2] = highByte(val);
    mydata[3] = lowByte(val);
    Serial.print("My data = ");
    for(int i=0;i<4;i++){
      Serial.print(mydata[i]);
    }
    Serial.println();
    sprintf(myStr, "0043%d%d", mydata[2],mydata[3]); 
    lora.sendUplink(mydata, strlen(mydata), 0);
    counter++;
  }

  recvStatus = lora.readData(outStr);
  if(recvStatus) {
    Serial.println(outStr);
  }
  
  // Check Lora RX
  lora.update();
}