Arduino LoRa.id SDK
====================
This repository contains the simple LoRaWAN library originally created by Ideetron B.V. This library is slightly
modified and encapsulated to run in the generic platform, allowing using the SX1272, SX1276 tranceivers and compatible modules (such as some HopeRF RFM9x modules).

To find out how to use the library itself, see the examples, or see the PDF file in the doc subdirectory.

Installing
----------
To install this library:

 - install it using the Arduino Library manager ("Sketch" -> "Include
   Library" -> "Manage Libraries..."), or
 - download a zipfile from github using the "Download ZIP" button and
   install it using the IDE ("Sketch" -> "Include Library" -> "Add .ZIP
   Library..."
 - clone this git repository into your sketchbook/libraries folder.

For more info, see https://www.arduino.cc/en/Guide/Libraries

Features
--------
The LoRa.id SDK library supports LoRaWAN Class A and Class C implementations operating in EU-868 and AS-923 bands. Note that this library is fairly simple with the aim of demonstrating the LoRa.id capabilities.

Configuration
-------------
A number of features can be configured or disabled by editing the
`config.h` file in the library folder. Unfortunately the Arduino
environment does not offer any way to do this (compile-time)
configuration from the sketch, so be careful to recheck your
configuration when you switch between sketches or update the library.

At the very least, you should set the right type of board in config.h, most other values should be fine at their defaults.

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
- Cytron Shield LoRa-RFM (https://www.cytron.io/p-shield-lora-rfm)
- Dragino (http://www.dragino.com/products/module/item/102-lora-shield.html)

This library has been tested using:
- Arduino Uno
- WeMos D1 R2 (ESP8266 family board)

Connections
-----------
To make this library work, your Arduino (or whatever Arduino-compatible
board you are using) should be connected to the transceiver. The exact
connections are a bit dependent on the transceiver board and Arduino
used, so this section tries to explain what each connection is for and
in what cases it is (not) required.

### DIO pins
The DIO (digitial I/O) pins on the transceiver board can be configured
for various functions. You need to specify the transceiver board in `config.h`. You can also specify custom DIO pins depending on your board.

```c
// Board definition, uncomment your board
// #define BOARD_DRAGINO_SHIELD
// #define BOARD_CYTRON_SHIELD
#define CUSTOM_BOARD

// Your custom pins
#ifdef CUSTOM_BOARD
    #define DIO0    2
    #define DIO1    6
    #define DIO5    8
    #define DIO2    7
    #define CS      10
    #define RFM_RST 9        
#endif
```
API
--------
This library provides a high-level API for connecting the device to Antares server.

#### LoRa module initialization
Initialize LoRa/LoRaWAN module. Must be called once in the Arduino setup block.

##### Syntax
```c
  void init(void);
```

##### Example
```c
void setup() {
  // Setup loraid access
  lora.init();
  ...
}
```

#### Setup Authentication Keys
Setup authentication keys provisioned by Antares server, including device address.

##### Syntax
```c
void setAccessKey(unsigned char *accessKey_in);
```

##### Example
```c
void setup() {
  // Setup loraid access
  lora.init();
  ...

  // Put Antares Key and DevAddress here
  lora.setAccessKey("8878f39f897b9a50:bd6b3446f4c13871");
  ...

}
```

#### Setup Device ID for Authentication 
Setup device ID for activating the device.

##### Syntax
```c
void setDeviceId(unsigned char *devAddr_in);
```

##### Example
```c
void setup() {
  // Setup loraid access
  lora.init();
  ...

  // Put Antares Key and DevAddress here
  lora.setDeviceId("00000001");
  ...

}
```

#### Set Device Class
Set class of the device (Class A or Class C). Input as `CLASS_A` or `CLASS_C` enum.
##### Syntax
```c
void setDeviceClass(devclass_t dev_class);
```

##### Example
```c
void setup() {
  // Setup loraid access
  lora.init();
  ...

  // Set LoRaWAN Class
  lora.setDeviceClass(CLASS_A);
  ...
}

```

#### Set Data Rate
You can set data rate allowed in your region (AS_923 or EU_868).

| data_rate | Name | Config          |
|-----------|------|-----------------|
| 0         | DR0  | SF12 BW 125 KHz |
| 1         | DR1  | SF11 BW 125 KHz |
| 2         | DR2  | SF10 BW 125 KHz |
| 3         | DR3  | SF9 BW 125 KHz  |
| 4         | DR4  | SF8 BW 125 KHz  |
| 5         | DR5  | SF7 BW 125 KHz  |
| 6         | DR6  | SF7 BW 250 KHz  |

##### Syntax
```c
void setDataRate(unsigned char data_rate);
```

##### Example
```c
void setup() {
  // Setup loraid access
  lora.init();
  ...

  // Set Data Rate to SF10 BW 125 KHz
  lora.setDataRate(2);
}

```

#### Send data to Antares
You need to specify the length of data you want to send and also the message type (unconfirmed or confirmed message). Set `confirm = 0` to send unconfirmed message and `confirm = 1`' to send confirmed message.

#### Syntax
```c
void sendToAntares(unsigned char *data, unsigned int len, unsigned char confirm);
```        

##### Example
```c
void loop() {
  // put your main code here, to run repeatedly:
  char myStr[] = "Ini data LoRaku";  

  lora.sendToAntares(myStr, strlen(myStr), 0);
  ...

} 
```


#### Update and run LoRa FSM
Update and run the LoRa Finite State Machine (FSM). This line should be put inside the Arduino `loop` block.
##### Syntax
```c
void update(void);
```

##### Example
```c
void loop() {
  ...

  // Check Lora RX
  lora.update();
}

```

#### Check and retrieve incoming data
Check for the latest incoming data from server either in binary or string format. You need to provide char buffer to read the data.
##### Syntax
```c
void readData(void);
```

##### Example
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

 - `loraid-send-class-A.ino` shows basic usage of Class A lora.id SDK.
 - `loraid-send-class-C.ino` shows basic usage of Class C lora.id SDK.

License
-------
Most source files in this repository are made available under the
MIT License. The examples which use a more liberal
license. Some of the AES code is available under the LGPL. Refer to each
individual source file for more details.