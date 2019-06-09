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

#define IO_LOOP_DELAY (70000)
#define ARR_SIZE (50)

int current = 0;
int last = 0;
unsigned long lastUpdate = 0;

int dataArr[ARR_SIZE];

// set up the 'command' feed
AdafruitIO_Feed *command = io.feed("command");

void setup() {

  // start the serial connection
  Serial.begin(115200);

  pinMode(A0, OUTPUT);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // set up a message handler for the 'command' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  command->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  for (int i = 0; i < ARR_SIZE; i++) {
    dataArr[i] = 0;
  }
}

void loop() {
  analogWrite(A0, 0);
  // calculate avg every minute
  if (millis() > (lastUpdate + IO_LOOP_DELAY)) {
    int count = 0;
    int total = 0;
    for (int i = 0; i < ARR_SIZE; i++) {
      if (dataArr[i] != 0) {
        count++;
        total += dataArr[i];
      }
    }
    if (count != 0) {
      int avg = 1.0 * total / count;
      Serial.println(avg);
      analogWrite(A0, avg);
  
      // reset array
      for (int i = 0; i < ARR_SIZE; i++) {
        dataArr[i] = 0;
      }
    } else { // this should never reached (technically)
      Serial.println(0);
      analogWrite(A0, 0);
    }
    lastUpdate = millis();
  }
  
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
}

// this function is called whenever a 'command' message
// is received from Adafruit IO. it was attached to
// the command feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {
  // note: don't really care about output for this part.
  int resp = data->toInt();
  int range = map(resp, 0, 1024, 0, 102);

  if (range > 14) {
    // insert into arr
    if (range > 20) {
      range = 20;
    }
    int noiseLevel = map(range, 15, 20, 1, 5);
  
    for (int i = 0; i < ARR_SIZE; i++) {
      if (dataArr[i] == 0) {
        dataArr[i] = noiseLevel;
        break;
      }
    }
  }
  // analogWrite(D4, range);
}
