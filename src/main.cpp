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

void setup() {
  if(autoBrightness) pinMode(pResistor, INPUT);
  if(amPMMode) {
    pinMode(amPin, OUTPUT); 
    pinMode(pmPin, OUTPUT);
    digitalWrite(amPin, LOW);
    digitalWrite(pmPin, LOW);
  }
    
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

        Serial.print("pR: ");
        Serial.print(pResistorVal);
        Serial.print("  \tB: ");
        Serial.println(smoothBrightness);
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
    digitalWrite(amPin, (timeinfo.tm_hour < 12) ? HIGH : LOW);
    digitalWrite(pmPin, (timeinfo.tm_hour >= 12) ? HIGH : LOW);
  } 
  else hour = timeinfo.tm_hour * 100; 

  // Account for hour 0 in 24 hr mode
  if (hour != 0) display.showNumberDecEx(timeinfo.tm_min + hour, 0b01000000);
  else display.showNumberDecEx((timeinfo.tm_min + hour) * 10, 0b10000000, true, (uint8_t)4U, 1);

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void timeSync() {  
  // Connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  // delay(500);
  while (!time(nullptr)) {
    Serial.println(F("Waiting for time sync..."));
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
