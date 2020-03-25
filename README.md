![LibraryBuild](https://github.com/BeelanMX/Beelan-LoRaWAN/workflows/LibraryBuild/badge.svg?branch=master)

Arduino LoRaWAN library by Beelan
====================
This repository contains the simple LoRaWAN library originally created by Ideetron B.V. This library is slightly
modified and encapsulated to run in the generic platform, allowing using the SX1272, SX1276 transceivers and compatible modules (such as some HopeRF RFM9x modules).

This repository is all based on this [repo]( https://git.antares.id/lorawan-loraid/arduino-loraid), with the principal purpose to port it to the frequencies of america US915.

To find out how to use the library itself, see the examples, or see the PDF file in the doc subdirectory.

Features
--------
The arduino LoRaWAN library supports LoRaWAN Class A and Class C implementations operating in EU-868, AS-923 and US-915 bands. Note that this library is fairly simple with the aim of demonstrating the LoRaWAN capabilities.

What certainly works:
 - Sending packets uplink, taking into account duty cycling.
 - Custom frequencies and datarate settings.
 - Receiving downlink packets in the RX1 window (EU_868, AS923,US_915).
 - Over-the-air activation (OTAA / joining) (EU_868, AS923).
 - Class C operation.
 - Receiving downlink packets in the RX1 window (US_915).
 - Receiving downlink packets in the RX2 window.
 - Over-the-air activation (OTAA / joining) (US_915). 

What has not been tested:
 - Receiving and processing MAC commands.

If you try one of these untested features and it works, be sure to let
us know (creating a github issue is probably the best way for that).

Quick Installing
----------
To install, use the Arduino Library Manager and search for "Beelan LoraWAN" and install the library.

Manual Installing
----------
To install this library:

 - install it using the Arduino Library manager ("Sketch" -> "Include
   Library" -> "Manage Libraries..."), or
 - download a zipfile from github using the "Download ZIP" button and
   install it using the IDE ("Sketch" -> "Include Library" -> "Add .ZIP
   Library..."
 - clone this git repository into your sketchbook/libraries folder.

For more info, see https://www.arduino.cc/en/Guide/Libraries


Configuration
-------------
A number of features can be configured or disabled by editing the
`config.h` file in the library folder. Unfortunately the Arduino
environment does not offer any way to do this (compile-time)
configuration from the sketch, so be careful to recheck your
configuration when you switch between sketches or update the library.

At the very least, you should set the right type of board in config.h, most other values should be fine at their defaults.

When using the US_915 you need to select which sub-band you will use, by default it is sub-band 6.

Supported hardware
------------------
This library is intended to be used with plain LoRa transceivers,
connecting to them using SPI. In particular, the SX1272 and SX1276
families are supported (which should include SX1273, SX1277, SX1278 and
SX1279 which only differ in the available frequencies, bandwidths and
spreading factors). It has been tested with both SX1272 and SX1276
chips, using the Semtech SX1272 evaluation board and the HopeRF RFM92
and RFM95 boards (which supposedly contain an SX1272 and SX1276 chip
respectively).

Some of the supported pre-built board currently available in the market are:

- [Cytron Shield LoRa-RFM](https://www.cytron.io/p-shield-lora-rfm) 
- [Dragino](http://www.dragino.com/products/module/item/102-lora-shield.html) 
- [Electronic Cats CatWAN USB-Stick](https://www.tindie.com/products/electroniccats/catwan-usb-stick/)
- [Electronic Cats CatWAN Relay Board](https://www.tindie.com/products/electroniccats/catwan-relay-board/)
- [Electronic Cats CatWAN Shield](https://www.tindie.com/products/electroniccats/catwan-shield-lora-and-lorawan-for-arduino/)

This library has been tested using:

- Arduino Uno
- WeMos D1 R2 (ESP8266 family board)
- ESP32
- Electronic Cats CatWAN USB-Stick
- Electronic Cats Bast-WAN (Based on SAMR34)

Connections
-----------
To make this library work, your Arduino (or whatever Arduino-compatible
board you are using) should be connected to the transceiver. The exact
connections are a bit dependent on the transceiver board and Arduino
used, so this section tries to explain what each connection is for and
in what cases it is (not) required.


Pin mapping
-----------
As described above, most connections can use arbitrary I/O pins on the
Arduino side. To tell the arduino LoRaWAN library about these, a pin mapping struct is used in the sketch file.

For example, this could look like this:

	sRFM_pins RFM_pins = {
	  	.CS = SS,
	  	.RST = RFM_RST,
	  	.DIO0 = RFM_DIO0,
	  	.DIO1 = RFM_DIO1,
	  	.DIO2 = RFM_DIO2,
	  	.DIO5 = RFM_DIO5
  	}; 
  	
API
--------
This library provides a high-level API for connecting the device to Antares server.

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
  lora.setDataRate(MULTI);
}

```

### Set Data Rate
You can set data rate allowed in your region (AS_923, EU_868 or US915).

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

Examples
--------
This library currently provides two examples:

 - `send-class-A_B-ABP.ino` shows basic usage of Class A_B ABP activation.
 - `send-class-A_B-OTAA.ino` shows basic usage of Class A_B OTAA activation.

Tests
-------
 - See [Test Folder](test/README.md) 

Maintainer
-------

Beelan invests time and resources providing this open source design, please support Beelan!

License
-------
Most source files in this repository are made available under the
MIT License. The examples which use a more liberal
license. Some of the AES code is available under the LGPL. Refer to each
individual source file for more details.

