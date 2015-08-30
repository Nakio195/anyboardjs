// Token's firmware working copy
// Code based on RFDUINO hardware, uses C char arrays

#include <ArduinoJson.h>
#include <RFduinoBLE.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <string.h>

// VARIABLES
uint8_t cmd;
int i;
bool connected;
int len;
char sendData[20];
uint8_t getData[20];
uint8_t last_sector_ID = 0;
uint8_t current_sector_ID = 0;

uint16_t r, g, b, color, colorTemp, lux;

// BOARD CONSTANTS
#define RED_LED_PIN   2
#define GREEN_LED_PIN 3
#define BLUE_LED_PIN  4

/* Initialise color sensor with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();
/* Initialise color sensor with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

// TOKEN FIRMWARE METADATA
#define NAME    "AnyBoard Pawn"
#define VERSION "0.1"
#define UUID    "2071-5c87-364g"

// COMMANDS
const uint8_t GET_NAME             = 32;
const uint8_t GET_VERSION          = 33;
const uint8_t GET_UUID             = 34;
const uint8_t GET_BATTERY_STATUS   = 35;
const uint8_t GET_TEMPERATURE      = 36;
const uint8_t HAS_LED              = 64;
const uint8_t HAS_LED_COLOR        = 65;
const uint8_t HAS_VIBRATION        = 66;
const uint8_t HAS_COLOR_DETECTION  = 67;
const uint8_t HAS_LED_SCREEN       = 68;
const uint8_t HAS_RFID             = 71;
const uint8_t HAS_NFC              = 72;
const uint8_t HAS_ACCELEROMETER    = 73;
const uint8_t HAS_TEMPERATURE      = 74;
const uint8_t LED_OFF              = 128;
const uint8_t LED_ON               = 129;
const uint8_t LED_BLINK            = 130;
const uint8_t READ_COLOR           = 136;
const uint8_t MOVE                 = 194;

void setup() {
	// Enable outputs.
	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(BLUE_LED_PIN, OUTPUT);

	// Enable serial debug, type cntrl-M at runtime.
	Serial.begin(9600);
	Serial.println("RFduino example started");
	Serial.println("Serial rate set to 9600 baud");

	// Turn Off all LEDs initially
	digitalWrite(RED_LED_PIN, LOW);
	digitalWrite(GREEN_LED_PIN, LOW);
	digitalWrite(BLUE_LED_PIN, LOW);;

	// configure the RFduino BLE properties
	RFduinoBLE.advertisementData = "ledbtn";
	RFduinoBLE.advertisementInterval = 500;
	RFduinoBLE.deviceName = "RFduino";
	RFduinoBLE.txPowerLevel = -20;
	Serial.println("RFduino BLE Advertising interval is 500ms");
	Serial.println("RFduino BLE DeviceName: RFduino");
	Serial.println("RFduino BLE Tx Power Level: -20dBm");

	// start the BLE stack
	RFduinoBLE.begin();
	Serial.println("RFduino BLE stack started");

}

void loop()
{   
    //Read color codes, based on the code in https://learn.adafruit.com/adafruit-color-sensors/programming

      tcs.getRawData(&r, &g, &b, &color);
      colorTemp = tcs.calculateColorTemperature(r, g, b);
      Serial.print("DEBUG: Color: "); Serial.print(color, DEC); Serial.print(" ");
      Serial.println(" ");

      //Color hex translation to sector IDs
      // Sector_NAMES 	Sector_ID	                Color_ID (approx value given by the sensor)
      // START	 		1			12228
      // STOP	 		2			5737
      // A				3			18330
      // B				4			9560
      // C				5			8550
      // D				6			6806
      // E				7			5920
      // F				8			15454

      if (color > 12000 && color < 13000)
        current_sector_ID = 1;
      else if (color > 5300 && color < 5800)
        current_sector_ID = 2;
      else if (color > 18000 && color < 19000)
        current_sector_ID = 3;
      else if (color > 9000 && color < 10000)
        current_sector_ID = 4;
      else if (color > 8000 && color < 9000)
        current_sector_ID = 5;
      else if (color > 6500 && color < 7500)
        current_sector_ID = 6;
      else if (color > 5801 && color < 6300)
        current_sector_ID = 7;
      else if (color > 15000 && color < 16000)
        current_sector_ID = 8;


      //Sends sectors ID of the sector that has been left and the sector that has been reached in formatted JSON
      if (current_sector_ID != last_sector_ID)
      {
      	sendData[0] = MOVE;
      	sendData[1] = current_sector_ID;
      	sendData[2] = last_sector_ID;
        RFduinoBLE.send((char*) sendData, 3);

        //Update sector_ID variables
        last_sector_ID = current_sector_ID;
      }
	delay(300);
}

// Turns on the LED on a specific color: r=red, g=gree, etc..
void ledOn(int r, int g, int b) {
    if (r > 200) {
        digitalWrite(RED_LED_PIN, HIGH);
    } else {
        digitalWrite(RED_LED_PIN, LOW);
    }
    if (g > 200) {
        digitalWrite(GREEN_LED_PIN, HIGH);
    } else {
        digitalWrite(GREEN_LED_PIN, HIGH);
    }
    if (b > 200) {
        digitalWrite(BLUE_LED_PIN, HIGH);
    } else {
        digitalWrite(BLUE_LED_PIN, HIGH);
    }
}

void ledOff() {
    ledOn(0,0,0);
}

void ledBlink(int r, int g, int b, int delayTime) {
    ledOn(r, g, b);
    delay(delayTime*10);
    ledOff();
    delay(delayTime*10);
    ledOn(r, g, b);
    delay(delayTime*10);
    ledOff();
    delay(delayTime*10);
    ledOn(r, g, b);
    delay(delayTime*10);
    ledOff();
    delay(delayTime*10);
    ledOn(r, g, b);
    delay(delayTime*10);
    ledOff();
    delay(delayTime*10);
    ledOn(r, g, b);
    delay(delayTime*10);
    ledOff();
    delay(delayTime*10);
    ledOn(r, g, b);
    // TODO: Hvorfor slutter denne ikke på?
}


void RFduinoBLE_onAdvertisement()
{
	Serial.println("RFduino is doing BLE advertising ...");
	digitalWrite(RED_LED_PIN, LOW);
	digitalWrite(GREEN_LED_PIN, LOW);
	digitalWrite(BLUE_LED_PIN, LOW);
}

void RFduinoBLE_onConnect()
{
	Serial.println("Connected");
  	connected = true;

}

void RFduinoBLE_onDisconnect()
{
	Serial.println("Disconnected");
   	connected = false;
}

void send(char *data, int length) {
    // TODO: impl
}

void RFduinoBLE_onReceive(char *data, int length) {
    cmd = data[0];
    for (i = 1; i < length; i++)
    {
        getData[i-1] = data[i];
    }
    parse(cmd);
}


void parse(uint8_t command) {

    switch (command) {
        case GET_NAME:
            Serial.println("GET_NAME");
            len = strlen(NAME);
            send(NAME, len);
            break;
        case GET_VERSION:
            Serial.println("GET_VERSION");
            len = strlen(VERSION);
            send(VERSION, len);
            break;
        case GET_UUID:
            Serial.println("GET_UUID");
            len = strlen(UUID);
            send(UUID, len);
            break;
        case LED_ON:
            Serial.println("LED_ON");
            ledOn(getData[0], getData[1], getData[2]);
            break;
        case LED_OFF:
            Serial.println("LED_OFF");
            ledOff();
            break;
        case LED_BLINK:
            Serial.println("LED_BLINK");
            ledBlink(getData[0], getData[1], getData[2], getData[3]);
            break;
        case GET_BATTERY_STATUS:
            Serial.println("GET_BATTERY_STATUS");
            sendData[0] = 0;
            send(sendData, 1);
            break;
        case HAS_LED:
            Serial.println("HAS_LED");
            sendData[0] = 1;
            send(sendData, 1);
            break;
        case HAS_LED_COLOR:
            Serial.println("HAS_LED_COLOR");
            sendData[0] = 1;
            send(sendData, 1);
            break;
        case HAS_VIBRATION:
            Serial.println("HAS_VIBRATION");
            sendData[0] = 0;
            send(sendData, 1);
            break;
        case HAS_COLOR_DETECTION:
            Serial.println("HAS_COLOR_DETEC");
            sendData[0] = 1;
            send(sendData, 1);
            break;
        case HAS_LED_SCREEN:
            Serial.println("HAS_LED_SCREEN");
            sendData[0] = 0;
            send(sendData, 1);
            break;
        case HAS_RFID:
            Serial.println("HAS_RFID");
            sendData[0] = 0;
            send(sendData, 1);
            break;
        case HAS_NFC:
            Serial.println("HAS_NFC");
            sendData[0] = 0;
            send(sendData, 1);
            break;
        case HAS_ACCELEROMETER:
            Serial.println("HAS_ACCELEROMET");
            sendData[0] = 0;
            send(sendData, 1);
            break;
        case HAS_TEMPERATURE:
            Serial.println("HAS_TEMPERATURE");
            sendData[0] = 0;
            send(sendData, 1);
            break;
        default:
            Serial.print("command not supported: ");
            Serial.print(command);
            Serial.print("\n");
    }
}