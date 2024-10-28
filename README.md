![LibraryBuild](https://github.com/BeelanMX/Beelan-LoRaWAN/workflows/LibraryBuild/badge.svg?branch=master)

Arduino LoRaWAN library
====================
This repository contains the simple LoRaWAN library encapsulated to run in the generic platform, allowing using the SX1272, SX1276 transceivers and compatible modules (such as some HopeRF RFM9x modules).

This repository is all based on the library originally created by [Ideetron B.V.](https://github.com/Ideetron) This library is slightly
modified and this [repo]( https://git.antares.id/lorawan-loraid/arduino-loraid), with the principal purpose to have an LoRaWAN MAC layer for arduino with an easy API.

<a href="https://github.com/ElectronicCats/Beelan-LoRaWAN/wiki">
  <img src="https://github.com/ElectronicCats/flipper-shields/assets/44976441/6aa7f319-3256-442e-a00d-33c8126833ec" width="200" height="104" />
</a>

Features
--------
The Arduino LoRaWAN library supports LoRaWAN Class A and Class C implementations operating in EU-868, AS-923, US-915 and AU-915 bands. Notice that this library is fairly simple with the aim of demonstrating the LoRaWAN capabilities.

### What certainly works:
 - Sending packets uplink, taking into account duty cycling.
 - Custom frequencies and data rate settings.
 - Receiving downlink packets in the RX1 window (EU-868, AS-923,US-915,AU-915).
 - Over-the-air activation (OTAA / joining) (EU-868, AS-923).
 - Class C operation.
 - Receiving downlink packets in the RX1 window (US-915).
 - Receiving downlink packets in the RX2 window.
 - Over-the-air activation (OTAA / joining) (US-915). 

### What has not been tested:
 - Receiving and processing MAC commands.

If you try one of these untested features and it works, be sure to let
us know (creating a Github issue is probably the best way for that, also so you can submit a pull request to improve this repository).

Installing the library
----------
Please refer to the wiki's [first steps](https://github.com/ElectronicCats/Beelan-LoRaWAN/wiki/2.-First-steps) section for instructions on installing the library.

Configuration
-------------
A number of features can be configured or disabled by editing the
`config.h` file in the library folder. Unfortunately the Arduino
environment does not offer any way to do this (compile-time)
configuration from the sketch, so be careful to recheck your
configuration when you switch between sketches or update the library.

At the very least, you should set the right type of board in config.h, most other values should be fine at their defaults.

When using the US_915 you need to select which sub-band you will use, by default it is sub-band 6.

We have to manually set the frequency plan (EU_868, US_915, AS_923, AS_923_2...) in the Config.h of the library.

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

- [Electronic Cats BastWAN](https://electroniccats.com/store/bastwan/)
- [Electronic Cats CatWAN USB-Stick](https://electroniccats.com/store/catwan-usb-stick/)
- [Electronic Cats CatWAN Relay Board](https://electroniccats.com/store/catwan-relay-board/)
- [Electronic Cats CatWAN Shield](https://electroniccats.com/store/loracatshield/)
- [Cytron Shield LoRa-RFM](https://www.cytron.io/p-shield-lora-rfm) 
- [Dragino](http://www.dragino.com/products/module/item/102-lora-shield.html) 


This library has been tested using:

- Arduino Uno
- WeMos D1 R2 (ESP8266 family board)
- ESP32
- Electronic Cats CatWAN USB-Stick
- Electronic Cats Bast-WAN (Based on SAMR34)
- NINA B302 is NRF52840 core
- STMDuino
- RENESAS compatible boards

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
 - See [API.md](API.md).

Tests
-------
 - See [Test Folder](test/README.md) 

## How to contribute <img src="https://electroniccats.com/wp-content/uploads/2018/01/fav.png" height="35"><img src="https://raw.githubusercontent.com/gist/ManulMax/2d20af60d709805c55fd784ca7cba4b9/raw/bcfeac7604f674ace63623106eb8bb8471d844a6/github.gif" height="30">
 Contributions are welcome! 

Please read the document  [**Contribution Manual**](https://github.com/ElectronicCats/electroniccats-cla/blob/main/electroniccats-contribution-manual.md)  which will show you how to contribute your changes to the project.

✨ Thanks to all our [contributors](https://github.com/ElectronicCats/Beelan-LoRaWAN/graphs/contributors)! ✨

See  [**_Electronic Cats CLA_**](https://github.com/ElectronicCats/electroniccats-cla/blob/main/electroniccats-cla.md) for more information.

See the [**community code of conduct**](https://github.com/ElectronicCats/electroniccats-cla/blob/main/electroniccats-community-code-of-conduct.md)   for a vision of the community we want to build and what we expect from it.

Maintainer
-------

Electronic Cats invests time and resources providing this open source design, please support Electronic Cats and open-source hardware by purchasing products from Electronic Cats!
Thanks Beelan

<a href="https://github.com/sponsors/ElectronicCats">
 <p align="center">
  <img src="https://electroniccats.com/wp-content/uploads/2020/07/Badge_GHS.png" height="104" />
 </p>
</a>

License
-------
Most source files in this repository are made available under the
[MIT License](https://github.com/ElectronicCats/Beelan-LoRaWAN/blob/master/LICENSE.txt). The examples which use a more liberal
license. Some of the AES code is available under the LGPL. Refer to each
individual source file for more details.

