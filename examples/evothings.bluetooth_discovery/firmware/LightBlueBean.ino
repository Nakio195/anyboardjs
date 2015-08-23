//
// Copyright 2014, Evothings AB
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// LightBlue Bean - Basic
//
// Created October 28, 2014
// Andreas Lundquist, Evothings AB
//
// This example shows how to use the Scratch data areas
// to communicate with an external application.
//
// In this example the sketch fetches the LED values from
// the scratch data area 1 and writes the current temperature
// to the scratch data area 2.
//

String beanName = "LightBlueBean";
const uint8_t ledScratch = 1;
const uint8_t temperatureScratch = 2;
#define NAME          "AnyToken 1"
#define VERSION       "0.1"

#define GET_NAME      1
#define GET_VERSION   2
#define LED_ON        65

void setup() {
        Serial.begin(9600);
	// Setup bean
	Bean.setBeanName(beanName);
	Bean.enableWakeOnConnect(true);

	// Reset the scratch data area 1.
	uint8_t resetLedBuffer[] = {0, 0, 0};
	Bean.setScratchData(ledScratch, resetLedBuffer, 3);
}

// the loop routine runs over and over again forever:
void loop() {
	bool connected = Bean.getConnectionState();

	if(connected) {
		// Write current temperature to a scratch data area.
		uint8_t temperatureBuffer[1];
		temperatureBuffer[0] = Bean.getTemperature();
		Bean.setScratchData(temperatureScratch, temperatureBuffer, 1);

		// Update LEDs
		ScratchData receivedData = Bean.readScratchData(ledScratch);

		uint8_t cmd = receivedData.data[0];
                if (cmd == GET_NAME) {
                  Serial.println(NAME);
                }
                if (cmd == GET_VERSION) {
                  Serial.println(VERSION);
                }
                if (cmd == LED_ON) {
                  Bean.setLed(receivedData.data[1], receivedData.data[2], receivedData.data[3]);
                } 
	}
	else {
		// Turn LED off and put to sleep.
		Bean.setLed(0, 0, 0);
		Bean.sleep(0xFFFFFFFF);
	}
}

void ledON(int r, int g, int b) {
  Bean.setLed(r, g, b);
}