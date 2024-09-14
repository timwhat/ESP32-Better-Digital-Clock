// fill out your wifi credentials (EDIT)
const char* ssid = "";
const char* password = "";

// Time sync replace with your timezone and dst from here https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char* ntpServer = "time.google.com";
const char* timezone = "EST5EDT,M3.2.0,M11.1.0"; // (EDIT)

#define refreshTimeInterval 1000

#define timeSyncInterval 3600000

// Define the LED display connection pins
#define CLK 27
#define DIO 14

// ENABLE AUTO BRIGHTNESS/Define Photoresistor Pin, Analog Pin Required
#define autoBrightness false // false to disable
#define pResistor A5
#define offsetBrightness 0 // -7 - 7
#define preSetBrightness 1 // 0-7

// ENABLE AM/PM mode, any pins can be used
#define amPMMode false
#define amPin 26 
#define pmPin 25
