/*
 * Example of OTAA class C device
 */
#include <lorawan.h>

// OTAA credentials
const char *devEui = "0000000000000000";
const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

const unsigned long interval = 10000; // 10 s interval to send message
unsigned long previousMillis = 0;     // will store last time message sent
unsigned int counter = 0;             // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

//Pins used to handle the communication with the LoRa module
const sRFM_pins RFM_pins = {
  .CS = 7,
  .RST = 3, 
  .DIO0 = 18,
  .DIO1 = 19, 
  .DIO2 = -1, 
  .DIO5 = -1,
};

void setup() {
  // Setup loraid access
  Serial.begin(115200);
  while(!Serial);
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    delay(10000);
    return;
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_C);

  // Set Data Rate
  lora.setDataRate(SF8BW125);

  // set channel to random
  lora.setChannel(MULTI);

  // Put OTAA Key and DevAddress here
  lora.setDevEUI(devEui);
  lora.setAppEUI(appEui);
  lora.setAppKey(appKey);

  // Join procedure
  bool isJoined;
  do {
    Serial.println("Joining...");
    isJoined = lora.join();
    
    //wait for 10s to try again
    delay(10000);
  }while(!isJoined);
  Serial.println("Joined to network");
}

void loop() {
  // Check interval overflow
  if (millis() - previousMillis > interval){
    previousMillis = millis(); 

    sprintf(myStr, "Counter-%d", counter); 

    Serial.print("Sending: ");
    Serial.println(myStr);

    //To send the string, the message is confirmed, using Port1
    lora.sendUplink(myStr, strlen(myStr), 1, 1); 
    counter++;
  }

  //If any downliks are received from the server, proceed to read and print it, as well as the reception power
  recvStatus = lora.readData(outStr);
    if(recvStatus) {
      Serial.println(lora.getRssi());
      Serial.println(outStr);
    }

  // Check Lora RX
  lora.update();

  //To receive the "Acknowlede" message from the server, since our Uplink was a confirmed one
  if(lora.readAck()) Serial.println("ack received");
}
