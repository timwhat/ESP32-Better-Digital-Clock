// Remove the example in the filename before use

// Debug stuff
#define SERIAL_DEBUG true // false to disable serial output

// Fill out your wifi credentials (EDIT)
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
#define amPMMode false // false to disable and it will use 24 hour time
#define amPin 26 
#define pmPin 25

// RGB LED Pins (must disable old LED pins if enabled)
#define RGBLED false // false to disable
#define rgbLEDType 0 // 0 for common anode, 1 for common cathode

// RGB Modes (WIP will add more modes)
#define RGBMode 0 // 0 for static color 

// Define RGB color for static mode with individual 0-255 values (EDIT)
#define staticColorRed 255   // Red value (0-255)
#define staticColorGreen 0   // Green value (0-255)
#define staticColorBlue 0    // Blue value (0-255)

// RGB LED Pins (EDIT)
#define amLEDRed 16
#define amLEDGreen 17
#define amLEDBlue 18

#define pmLEDRed 19
#define pmLEDGreen 21
#define pmLEDBlue 22