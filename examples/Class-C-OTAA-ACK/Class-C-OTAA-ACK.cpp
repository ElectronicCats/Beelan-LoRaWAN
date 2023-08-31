
#include <Arduino.h>
#include <lorawan.h>

// OTAA credentials
const char *devEui = "0";
const char *appEui = "0";
const char *appKey = "0";

const unsigned long interval = 15 * 1000; // 10 s interval to send message
unsigned long previousMillis = 0;     // will store last time message sent
unsigned int counter = 0;             // message counter

bool sendack = false;

char myStr[50];

const sRFM_pins RFM_pins = {
    .CS = 7,
    .RST = 3,
    .DIO0 = 18,
    .DIO1 = 19,
};

void message(sBuffer *msg, bool isConfirmed, uint8_t fPort){

  char Buff[255];
  int size = msg->Counter;

  memset(Buff, 0x00, size + 1);
  memcpy(Buff, msg->Data, size);

  Serial.println("--------------------");
  Serial.print("Msg size as bytes : ");
  Serial.println(msg->Counter);
  Serial.print("Message :");
  Serial.println(Buff);
  Serial.print("Port :");
  Serial.println(fPort);

  if(isConfirmed){

    Serial.println("ACK response Should be sent !");
    sendack = true;
    
  }

}

void setup() {
  // Setup loraid access
  Serial.begin(115200);
  while(!Serial);
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    delay(10000);
    return;
  }

  // Set Data Rate
  lora.setDataRate(SF8BW125);

  // set channel to random
  lora.setChannel(MULTI);
  // Put OTAA Key and DevAddress here
  lora.setDevEUI(devEui);
  lora.setAppEUI(appEui);
  lora.setAppKey(appKey);

  // Set Callback function
  lora.onMessage(message);

  // Join procedure
  bool isJoined;
  do {
    Serial.println("Joining...");
    isJoined = lora.join();
    
    //wait for 10s to try again
    delay(7000);
  }while(!isJoined);
  Serial.println("Joined to network");

    delay(1000);

    sprintf(myStr, "First Message"); 

    Serial.print("Sending: ");
    Serial.println(myStr);
    
    lora.sendUplink(myStr, strlen(myStr), 0, 1);
    counter++;

}

void loop() {

  // Check interval overflow
  if (millis() - previousMillis > interval){
    delay(1000);
 
    previousMillis = millis(); 

    sprintf(myStr, "Counter-%d", counter); 

    Serial.print("Sending: ");
    Serial.println(myStr);
    
    lora.sendUplink(myStr, strlen(myStr), 1, 1);
    counter++;

  }

  if (sendack) {

    lora.sendACK();
    sendack = false;

  }

  lora.update();

  if(lora.readAck()) Serial.println("ack received");

}
