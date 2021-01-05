
### LoRa module initialization
Initialize LoRa/LoRaWAN module. Must be called once in the Arduino setup block.

#### Syntax
```c
  void init(void);
```

#### Example
```c
void setup() {
  // Setup loraid access
  lora.init();
  ...
}
```

### Setup Authentication Keys for ABP activation
Setup authentication keys for your LoRaWAN device, including device address.

#### Syntax
```c
void setAppSKey(unsigned char *ApskKey_in);
void setNwkSKey(unsigned char *NwkKey_in);
```

#### Example
```c
void setup() {
  // Setup loraid access
  if(!lora.init()){
  Serial.println("RFM95 not detected");
  while(1);
  }
  ...

  // Put Antares Key and DevAddress here
  lora.setNwkSKey("b7300d9f68b649ed30530f9dd69f9afe");
  lora.setAppSKey("9d52eef7fab63eda18794d0e503ddf20");
  ...

}
```

### Setup Device address
Setup device address for activating the device.

#### Syntax
```c
void setDevAddr(unsigned char *devAddr_in);
```

#### Example
```c
void setup() {
  // Setup loraid access
  if(!lora.init()){
  Serial.println("RFM95 not detected");
  while(1);
  }
  ...

  // Put Antares Key and DevAddress here
  lora.setDevAddr("07000007");
  ...

}
```

### Setup Authentication Keys for OTAA activation
Setup authentication keys for your LoRaWAN device, including device address.

#### Syntax
```c
void setDevEUI(const char *devEUI_in);
void setAppEUI(const char *appEUI_in);
void setAppKey(const char *appKey_in);
```

#### Example
```c
void setup() {
  // Setup loraid access
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    while(1);
  }
  ...

  // Put OTAA credentials here
  lora.setDevEUI("b7300d9f68b649ed");
  lora.setAppEUI("8b649ed30530f9dd");
  lora.setAppKey("9d52eef7fab63eda18794d0e503ddf20");
  ...

}
```
### Join Procedure
Need to join in the network

#### Syntax
```c
bool join();
```

#### Example
```c
void setup() {
  // Setup loraid access
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    while(1);
  }

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
```


### Set Device Class
Set class of the device (Class A or Class C). Input as `CLASS_A` or `CLASS_C` enum.
#### Syntax
```c
void setDeviceClass(devclass_t dev_class);
```

#### Example
```c
void setup() {
  // Setup loraid access
  if(!lora.init()){
  Serial.println("RFM95 not detected");
  while(1);
  }
  ...

  // Set LoRaWAN Class
  lora.setDeviceClass(CLASS_A);
  ...
}

```
### Set Channel
You can set channel allowed in your region (AS_923, EU_868 or US915).
 - For US_915 the channels can be [0 - 7]
 - Use MULTI if you want random channel

### Syntax
```c
void setChannel(unsigned char channel);
```
#### Example
```c
void setup() {
  // Setup loraid access
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    while(1);
  }
  ...

  // Set random Channel
  lora.setChannel(MULTI);
}

```

### Set Data Rate
You can set data rate allowed in your region (AS_923, EU_868 or US915).

## For AU915
| data_rate    | Name  | Config          | Direction   
|--------------|-------|-----------------|----------
| 0            | DR0   | SF12 BW 125 KHz | Uplink
| 1            | DR1   | SF11 BW 125 KHz | Uplink
| 2            | DR2   | SF10 BW 125 KHz | Uplink
| 3            | DR3   | SF9 BW 125 KHz  | Uplink
| 4            | DR4   | SF8 BW 500 KHz  | Uplink
| 5            | DR5   | SF7 BW 500 KHz  | Uplink
| 6            | DR6   | SF8 BW 500 KHz  | Uplink
| 7  	       | RFU   | N/A		  | N/A
| 8            | DR8   | SF12 BW 500 KHz  | Downlink
| 9            | DR9   | SF11 BW 500 KHz  | Downlink
| 10           | DR10  | SF10 BW 500 KHz  | Downlink
| 11           | DR11  | SF9 BW 500 KHz  | Downlink
| 12           | DR12  | SF8 BW 500 KHz  | Downlink
| 13           | DR13  | SF7 BW 500 KHz  | Downlink
| 14           | RFU  | N/A  | N/A
| 15           | Defined in LoRaWAN  | N/A  | 

For AU915 is important to remark that DR0-DR5 are only for UPLINKS
and DR8-DR13 are only for DOWNLINKS


##	For AS923 or EU868
| data_rate | Name | Config          |Direction
|-----------|------|-----------------|----------------
| 0         | DR0  | SF12 BW 125 KHz | Uplink/Downlink
| 1         | DR1  | SF11 BW 125 KHz | Uplink/Downlink
| 2         | DR2  | SF10 BW 125 KHz | Uplink/Downlink
| 3         | DR3  | SF9 BW 125 KHz  | Uplink/Downlink
| 4         | DR4  | SF8 BW 125 KHz  | Uplink/Downlink
| 5         | DR5  | SF7 BW 125 KHz  | Uplink/Downlink
| 6         | DR6  | SF7 BW 250 KHz  | Uplink/Downlink

## For US915
| data_rate    | Name  | Config          | Direction   
|--------------|-------|-----------------|----------
| 0            | DR0   | SF10 BW 125 KHz | Uplink
| 1            | DR1   | SF9 BW 125 KHz | Uplink
| 2            | DR2   | SF8 BW 125 KHz | Uplink
| 3            | DR3   | SF7 BW 125 KHz  | Uplink
| 4            | DR4   | SF8 BW 500 KHz  | Uplink
| 5:7  	     | RFU   | 		N/A		    | N/A
| 8            | DR8   | SF12 BW 500 KHz  | Downlink
| 9            | DR9   | SF11 BW 500 KHz  | Downlink
| 10           | DR10  | SF10 BW 500 KHz  | Downlink
| 11           | DR11  | SF9 BW 500 KHz  | Downlink
| 12           | DR12  | SF8 BW 500 KHz  | Downlink
| 13           | DR13  | SF7 BW 500 KHz  | Downlink

For US915 is important to remark that DR0-DR4 are only for UPLINKS
and DR8-DR10 are only for DOWNLINKS

*RFU: Reserved for future use

### Syntax
```c
void setDataRate(unsigned char data_rate);
```

#### Example
```c
void setup() {
  // Setup loraid access
  if(!lora.init()){
  Serial.println("RFM95 not detected");
  while(1);
  }
  ...

  // Set Data Rate to SF10 BW 125 KHz
  lora.setDataRate(2);
}

```

### Send data to LoRaWAN
You need to specify the length of data you want to send and also the message type (unconfirmed or confirmed message). Set `confirm = 0` to send unconfirmed message and `confirm = 1`' to send confirmed message.

#### Syntax
```c
void sendUplink(unsigned char *data, unsigned int len, unsigned char confirm);
```        

#### Example
```c
void loop() {
  // put your main code here, to run repeatedly:
  char myStr[] = "Ini data LoRaku";  

  lora.sendUplink(myStr, strlen(myStr), 0);
  ...

} 
```


### Update and run LoRa FSM
Update and run the LoRa Finite State Machine (FSM). This line should be put inside the Arduino `loop` block.

#### Syntax
```c
void update(void);
```

#### Example
```c
void loop() {
  ...

  // Check Lora RX
  lora.update();
}

```

In this version we'll try to use interrupts in order to eliminate this FSM.

### Check and retrieve incoming data
Check for the latest incoming data from server either in binary or string format. You need to provide char buffer to read the data.
#### Syntax
```c
void readData(void);
```

#### Example
```c

char buffer_rx[255];

void setup() {
  ...
}

void loop() {
  int recvStatus;
  ...

  // LoRa FSM
  lora.update();

  // Check data
  recvStatus = lora.readData(buffer_rx);
  if(recvStatus) {
    Serial.println(buffer_rx);
  }
}
```
