#include <iostream>
#include <string>
#include <Arduino.h>
#include <TM1637Display.h>
#include <WiFi.h>
#include "time.h"
#include "config.h"

using namespace std;

void timeSync();
void printLocalTime();

TM1637Display display(CLK, DIO); // Create an instance of the TM1637Display

// For wifi sync to stop time drifting
unsigned long currentTime, syncPrevTime, updateDisplay;
int smoothBrightness = 0;
bool timeSyncc = false;

// Declare global variables to store previous values (improves efficiency)
int prevHour = -1;
int prevMinute = -1;
bool prevAmPmState = false;
bool zeroHour = false;

void setup() {
  if(autoBrightness) pinMode(pResistor, INPUT);
  #ifdef amPMMode
    pinMode(amPin, OUTPUT); 
    pinMode(pmPin, OUTPUT);
    digitalWrite(amPin, LOW);
    digitalWrite(pmPin, LOW);
    #ifdef RGBLED 
      pinMode(amLEDRed, OUTPUT);
      pinMode(amLEDGreen, OUTPUT);
      pinMode(amLEDBlue, OUTPUT);
      pinMode(pmLEDRed, OUTPUT);
      pinMode(pmLEDGreen, OUTPUT);
      pinMode(pmLEDBlue, OUTPUT);
    #endif
  #endif
    
  Serial.begin(9600);
  
  // Sync time with NTP server
  configTime(0, 0, ntpServer);
}

void loop() {
    currentTime = millis();

    // sync the time
    if ((currentTime - syncPrevTime) >= timeSyncInterval || timeSyncc == false) {
      syncPrevTime = currentTime;
      timeSync();
    }

    // Update the display
    if ((currentTime - updateDisplay) >= refreshTimeInterval) {
      updateDisplay = currentTime;
      if(!autoBrightness) display.setBrightness(preSetBrightness);
      else {
        int pResistorVal = constrain(floor((analogRead(pResistor) / 512) - offsetBrightness), 0, 7);

        smoothBrightness += (smoothBrightness < pResistorVal) - (smoothBrightness > pResistorVal);

        display.setBrightness(smoothBrightness);
        if(SERIAL_DEBUG) {
          Serial.print("pR: ");
          Serial.print(pResistorVal);
          Serial.print("  \tB: ");
          Serial.println(smoothBrightness);
        }
      }
      printLocalTime();
    }  
}

void printLocalTime() {

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    uint8_t data[] = {
      0x00, 0x00,
      SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
      SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F
    };
    display.setSegments(data);
    timeSyncc = false;
    return;
  }
  timeSyncc = true;
  int hour;

  // checks if amPMMode is active
  if (amPMMode) {
    if (timeinfo.tm_hour == 0 || timeinfo.tm_hour == 12) hour = 12;
    else if (timeinfo.tm_hour > 12) hour = timeinfo.tm_hour - 12;
    else hour = timeinfo.tm_hour;

    // Set AM/PM pins based on hour
    bool currentAmPmState = timeinfo.tm_hour < 12;
    if (currentAmPmState != prevAmPmState) {
      if (RGBLED) {
        writeTOLED(currentAmPmState, 1);
        writeTOLED(!currentAmPmState, 0);
      }
      else {
        digitalWrite(amPin, currentAmPmState ? HIGH : LOW); 
        digitalWrite(pmPin, !currentAmPmState ? HIGH : LOW);  
      }
      prevAmPmState = currentAmPmState;
    }
  } 
  else hour = timeinfo.tm_hour * 100; 

  // Account for hour 0 in 24 hr mode
  if (hour != 0) {
    if (hour != prevHour || timeinfo.tm_min != prevMinute) {
      display.showNumberDecEx(timeinfo.tm_min + hour, 0b01000000);
      prevHour = hour;
      prevMinute = timeinfo.tm_min;
      if (!zeroHour){
        zeroHour = true;
      }
    }
  } else {
    if (hour != prevHour || timeinfo.tm_min != prevMinute) {
      if (zeroHour) { // checks if its the first time it displays 0 so it resets display
        display.showNumberDecEx(0, 0b01000000);
        zeroHour = false;
      }
      display.showNumberDecEx((timeinfo.tm_min + hour) * 10, 0b10000000, true, (uint8_t)4U, 1);
      prevHour = hour;
      prevMinute = timeinfo.tm_min;
    }
  }

  if (SERIAL_DEBUG) Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void timeSync() {  
  // Connect to WiFi
  if (SERIAL_DEBUG) Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (SERIAL_DEBUG) Serial.print(".");
  }
  if (SERIAL_DEBUG) Serial.println(" CONNECTED");

  // delay(500);
  while (!time(nullptr)) {
    if (SERIAL_DEBUG) Serial.println(F("Waiting for time sync..."));
    delay(500);
  }

  // Apply timezone after time synchronization
  setenv("TZ", timezone, 1);
  tzset();

  printLocalTime();

  // Disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

#ifdef RGBLED
void writeTOLED(int amPM, float brightness) {
  if (amPM == 0) { // AM
    analogWrite(amLEDRed, staticColorRed * brightness);
    analogWrite(amLEDGreen, staticColorGreen * brightness);
    analogWrite(amLEDBlue, staticColorBlue * brightness);
  }
  else { // PM
    analogWrite(pmLEDRed, staticColorRed * brightness);
    analogWrite(pmLEDGreen, staticColorGreen * brightness);
    analogWrite(pmLEDBlue, staticColorBlue * brightness);
  }
}
#endif