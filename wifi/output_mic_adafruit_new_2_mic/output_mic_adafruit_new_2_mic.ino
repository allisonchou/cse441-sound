// Instructables Internet of Things Class sample code
// Circuit Triggers Internet Action
// A button press is detected and stored in a feed
// An LED is used as confirmation feedback
//
// Modified by Becky Stern 2017
// based on the Adafruit IO Digital Input Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-digital-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************ Adafruit IO Configuration *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    "Escargot"
#define IO_KEY         "481753a92e964d5db7099acf9f51cd90"

/******************************* WIFI Configuration **************************************/

#define WIFI_SSID       "University of Washington"
#define WIFI_PASS       ""

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/************************ Main Program Starts Here *******************************/
#include <ESP8266WiFi.h>
#include <AdafruitIO.h>
#include <Adafruit_MQTT.h>
#include <ArduinoHttpClient.h>

#include <SoftwareSerial.h>

// set up the 'command' feed (demo)
AdafruitIO_Feed *command = io.feed("command");

// set up lab feed
AdafruitIO_Feed *lab = io.feed("lab");

SoftwareSerial ESPserial(2, 3); // RX | TX

unsigned long curTime;

int maxNoise[2];

void setup() {

  ESPserial.begin(57600);
  
  // start the serial connection
  // Serial.begin(115200);
  
  // connect to io.adafruit.com
  // Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // set up a message handler for the 'command' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  command->onMessage(handleMessage);
  lab->onMessage(handleMessageLab);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    // Serial.print(".");
    delay(500);
  }

  curTime = millis();
  maxNoise[0] = 0;
  maxNoise[1] = 0;
  
  // we are connected
  // Serial.println();
  // Serial.println(io.statusText());
}

void loop() {
  
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // Updates every 10 sec if both mics have data to send.
  if (millis() >= curTime + 10000 && maxNoise[0] != 0 && maxNoise[1] != 0) {
    ESPserial.write(maxNoise[0]);
    ESPserial.write(maxNoise[1]);
    maxNoise[0] = 0;
    maxNoise[1] = 0;
    curTime = millis();
  }
}

// this function is called whenever a 'command' message
// is received from Adafruit IO. it was attached to
// the command feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {
  int resp = data->toInt();

  // bounds checking
  if (resp < 0) {
    resp = 100;
  }
  if (resp > 1024) {
    resp = 1024;
  }
  
  int adjustedResp = map(resp, 0, 1024, 1, 5);
  // Serial.println(resp);
  
  if (adjustedResp > maxNoise[0]) {
    maxNoise[0] = adjustedResp;
  }
}

// this function is called whenever a 'lab' message
// is received from Adafruit IO. it was attached to
// the command feed in the setup() function above.
void handleMessageLab(AdafruitIO_Data *data) {
  int resp = data->toInt();

  // bounds checking
  if (resp < 0) {
    resp = 100;
  }
  if (resp > 1024) {
    resp = 1024;
  }
  
  int adjustedResp = map(resp, 0, 1024, 1, 5);
  // Serial.println(resp);
  
  if (adjustedResp > maxNoise[1]) {
    maxNoise[1] = adjustedResp;
  }
}
