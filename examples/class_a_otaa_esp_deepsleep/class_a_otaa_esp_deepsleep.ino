/**
 * Example of OTAA device with ESP8266 or ESP32 using deep sleep wihtout losing LoRaWAN settings.
 * A minimum number of pin is also used in this example. For slighly faster processing,
 * DIO0 and DIO1 should be used.
 */
#include <Arduino.h>
#include <lorawan.h>

// OTAA credentials
const char *devEui = "0000000000000000";
const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

unsigned int counter = 0; // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

const sRFM_pins RFM_pins = {
    .CS = 15,
    .RST = 2, //Could be connected to the reset ESP pin of ESP8266, if so put -1
    .DIO0 = 0,
    .DIO1 = 4,
    .DIO2 = -1,
    .DIO5 = -1,
};

struct RTCRAM
{
    unsigned char DevEUI[8];
    unsigned char AppEUI[8];
    unsigned char AppKey[16];
    unsigned char DevNonce[2];
    unsigned char AppNonce[3];
    unsigned char NetID[3];
    unsigned char Address_Tx[4];
    unsigned char NwkSKey[16];
    unsigned char AppSKey[16];
    unsigned int Frame_Counter_Tx;
    sSettings LoRa_Settings;
    unsigned int counter;
};
#if defined(ESP32)
RTC_DATA_ATTR RTCRAM rtcRAM;
#else
RTCRAM rtcRAM;
#endif

void deepsleep(uint64_t us)
{
    Serial.println("We go to deep sleep");
    delay(10);
    lora.sleep();
#if defined(ESP8266)
    ESP.deepSleep(us, WAKE_RF_DISABLED);
#elif defined(ESP32)
    esp_sleep_enable_timer_wakeup(us);
    esp_deep_sleep_start();
#else
    Serial.print("Deep sleep not supported by this example");
#endif
}

void saveVariableRTC()
{
    memcpy(rtcRAM.DevEUI, lora.DevEUI, sizeof(rtcRAM.DevEUI));
    memcpy(rtcRAM.AppEUI, lora.AppEUI, sizeof(rtcRAM.AppEUI));
    memcpy(rtcRAM.AppKey, lora.AppKey, sizeof(rtcRAM.AppKey));
    memcpy(rtcRAM.DevNonce, lora.DevNonce, sizeof(rtcRAM.DevNonce));
    memcpy(rtcRAM.AppNonce, lora.AppNonce, sizeof(rtcRAM.AppNonce));
    memcpy(rtcRAM.NetID, lora.NetID, sizeof(rtcRAM.NetID));
    memcpy(rtcRAM.Address_Tx, lora.Address_Tx, sizeof(rtcRAM.Address_Tx));
    memcpy(rtcRAM.NwkSKey, lora.NwkSKey, sizeof(rtcRAM.NwkSKey));
    memcpy(rtcRAM.AppSKey, lora.AppSKey, sizeof(rtcRAM.AppSKey));
    memcpy(&rtcRAM.Frame_Counter_Tx, &lora.Frame_Counter_Tx, sizeof(rtcRAM.Frame_Counter_Tx));
    memcpy(&rtcRAM.LoRa_Settings, &lora.LoRa_Settings, sizeof(rtcRAM.LoRa_Settings));
    memcpy(&rtcRAM.counter, &counter, sizeof(rtcRAM.counter));
#if defined(ESP8266)
    ESP.rtcUserMemoryWrite(32, (uint32_t *)&rtcRAM, sizeof(rtcRAM));
#endif
}

void loadVariableRTC()
{
#if defined(ESP8266)
    ESP.rtcUserMemoryRead(32, (uint32_t *)&rtcRAM, sizeof(rtcRAM));
#endif
    memcpy(lora.DevEUI, rtcRAM.DevEUI, sizeof(rtcRAM.DevEUI));
    memcpy(lora.AppEUI, rtcRAM.AppEUI, sizeof(rtcRAM.AppEUI));
    memcpy(lora.AppKey, rtcRAM.AppKey, sizeof(rtcRAM.AppKey));
    memcpy(lora.DevNonce, rtcRAM.DevNonce, sizeof(rtcRAM.DevNonce));
    memcpy(lora.AppNonce, rtcRAM.AppNonce, sizeof(rtcRAM.AppNonce));
    memcpy(lora.NetID, rtcRAM.NetID, sizeof(rtcRAM.NetID));
    memcpy(lora.Address_Tx, rtcRAM.Address_Tx, sizeof(rtcRAM.Address_Tx));
    memcpy(lora.NwkSKey, rtcRAM.NwkSKey, sizeof(rtcRAM.NwkSKey));
    memcpy(lora.AppSKey, rtcRAM.AppSKey, sizeof(rtcRAM.AppSKey));
    memcpy(&lora.Frame_Counter_Tx, &rtcRAM.Frame_Counter_Tx, sizeof(rtcRAM.Frame_Counter_Tx));
    memcpy(&lora.LoRa_Settings, &rtcRAM.LoRa_Settings, sizeof(rtcRAM.LoRa_Settings));
    memcpy(&counter, &rtcRAM.counter, sizeof(rtcRAM.counter));
}

#if defined(ESP8266)
uint32_t magicBytes; // if previous data save on rtc, this equal 'magi' (183510009)
void loadMagicByte()
{
    ESP.rtcUserMemoryRead(127, &magicBytes, sizeof(magicBytes));
}

void saveMagicByte()
{
    magicBytes = 183510009;
    ESP.rtcUserMemoryWrite(127, &magicBytes, 4);
}
#endif

void setup()
{
    // Setup loraid access
    Serial.begin(9600);
    while (!Serial)
        ;
    if (!lora.init())
    {
        Serial.println("RFM95 not detected");
        delay(5000);
        deepsleep(10000000);
    }

    // Join procedure
    bool isJoined = true;
#if defined(ESP8266)
    loadMagicByte();
    if (magicBytes != 183510009)
#else
    if (rtcRAM.counter == 0)
#endif
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
        while (!isJoined)
        {
            Serial.println("Joining...");
            isJoined = lora.join();

            // wait for 10s to try again if not join
            if (!isJoined)
                delay(10000);
        }
        Serial.println("Joined to network!\nWe now save LoRaWAN information");
        delay(100); // Necessary for ESP32 in order to properly save data on RTC ram
        saveVariableRTC();
#if defined(ESP8266)
        saveMagicByte();
#endif
    }
    else
    {
        Serial.println("Previously joined network, we applied the save parameters");
        loadVariableRTC();
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
    if (recvStatus)
    {
        Serial.println(outStr);
    }
    delay(100); // Necessary for ESP32 in order to properly save data on RTC ram
    // Save pertinent information on RTC ram
    saveVariableRTC();

    // Deep sleep for 10 s
    deepsleep(10000000);
}

void loop()
{
    // We never get here
    Serial.print("Oups, you use this example without an ESP device");
    delay(10000);
}