/**
 * Example of OTAA device with ESP8266 using deep sleep wihtout losing LoRaWAN settings.
 * A minimum number of pin is also used in this example. For slighly faster processing,
 * DIO0 and DIO1 should be used.
 */
#include <Arduino.h>
#include <lorawan.h>

// OTAA credentials
const char *devEui = "0000000000000000";
const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

byte rtcMemoryPointerInitial = 32; // Intial position of the pointer
byte rtcMemoryPointer = 32; // Start at 32, because the first 128 bytes (128/4 = 32) are used by the ESP device during OTA, so those data will be lost during OTA


const sRFM_pins RFM_pins = {
  .CS = 20,
  .RST = -1,
  .DIO0 = -1,
  .DIO1 = -1,
  .DIO2 = -1,
  .DIO5 = -1,
};

void setup() {
  // Setup loraid access
  Serial.begin(9600);
  while(!Serial);
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    delay(5000);
    ESP.deepSleep(10000000, WAKE_RF_DISABLED);
  }

  // Join procedure
  bool isJoined = true;
  uint32_t magicBytes; //if previous data save on rtc, this equal 'magi' (183510009)
  ESP.rtcUserMemoryRead(127, (uint32_t *) &magicBytes, sizeof(magicBytes));
  if (magicBytes != 183510009)
  {
    Serial.println("Cold boot Join procedure needs to be performed");
    isJoined = false;
  }

  if (!isJoined)
  {
    // Set LoRaWAN Class change CLASS_A or CLASS_C
    lora.setDeviceClass(CLASS_A);
    // Set Data Rate
    lora.setDataRate(SF9BW125);
    // set channel to random
    lora.setChannel(MULTI);

    // Put OTAA Key and DevAddress here
    lora.setDevEUI(devEui);
    lora.setAppEUI(appEui);
    lora.setAppKey(appKey);
    while(!isJoined) 
    {
      Serial.println("Joining...");
      isJoined = lora.join();
        
      //wait for 10s to try again if not join
      if (!isJoined) 
        delay(10000);
    }
    Serial.println("Joined to network");
    ESP.rtcUserMemoryWrite(rtcMemoryPointer, (uint32_t *) &lora.OTAA_Data, sizeof(lora.OTAA_Data));
    rtcMemoryPointer += sizeof(lora.OTAA_Data) / 4;
    ESP.rtcUserMemoryWrite(rtcMemoryPointer, (uint32_t *) &lora.Session_Data, sizeof(lora.Session_Data));
    rtcMemoryPointer += sizeof(lora.Session_Data) / 4;
    ESP.rtcUserMemoryWrite(rtcMemoryPointer, (uint32_t *) &lora.LoRa_Settings, sizeof(lora.LoRa_Settings));
    rtcMemoryPointer += sizeof(lora.LoRa_Settings) / 4;
    magicBytes = 183510009;
    ESP.rtcUserMemoryRead(127, &magicBytes, 4);
  }
  else
  {
    ESP.rtcUserMemoryRead(rtcMemoryPointer, (uint32_t *) &lora.OTAA_Data, sizeof(lora.OTAA_Data));
    rtcMemoryPointer += sizeof(lora.OTAA_Data) / 4;
    ESP.rtcUserMemoryRead(rtcMemoryPointer, (uint32_t *) &lora.Session_Data, sizeof(lora.Session_Data));
    rtcMemoryPointer += sizeof(lora.Session_Data) / 4;
    ESP.rtcUserMemoryRead(rtcMemoryPointer, (uint32_t *) &lora.LoRa_Settings, sizeof(lora.LoRa_Settings));
    rtcMemoryPointer += sizeof(lora.LoRa_Settings) / 4;
    ESP.rtcUserMemoryRead(rtcMemoryPointer, (uint32_t *) &counter, sizeof(counter));
    rtcMemoryPointer += sizeof(counter) / 4;
  }

  // Send a upling
  sprintf(myStr, "Counter-%d", counter); 
  Serial.print("Sending: ");
  Serial.println(myStr);
  lora.sendUplink(myStr, strlen(myStr), 0, 1);
  counter++;

  // cycle over send and receive data with LoRa update
  lora.update();
  recvStatus = lora.readData(outStr);
  if(recvStatus) {
    Serial.println(outStr);
  }

  // Save pertinent information on RTC
  rtcMemoryPointer = rtcMemoryPointerInitial;
  ESP.rtcUserMemoryWrite(rtcMemoryPointer, (uint32_t *) &lora.OTAA_Data, sizeof(lora.OTAA_Data));
  rtcMemoryPointer += sizeof(lora.OTAA_Data) / 4;
  ESP.rtcUserMemoryWrite(rtcMemoryPointer, (uint32_t *) &lora.Session_Data, sizeof(lora.Session_Data));
  rtcMemoryPointer += sizeof(lora.Session_Data) / 4;
  ESP.rtcUserMemoryWrite(rtcMemoryPointer, (uint32_t *) &lora.LoRa_Settings, sizeof(lora.LoRa_Settings));
  rtcMemoryPointer += sizeof(lora.LoRa_Settings) / 4;
  ESP.rtcUserMemoryWrite(rtcMemoryPointer, (uint32_t *) &counter, sizeof(counter));

  // Deep sleep for 10 s
  lora.sleep();
  ESP.deepSleep(10000000, WAKE_RF_DISABLED);
}

void loop() {
  // We never get here
}
