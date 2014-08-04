#include <OneWire.h>
#include <LiquidCrystal.h>
#include <Wire.h>

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(4);  // on pin 10 (a 4.7K resistor is necessary)

byte addr0[] = {0x28, 0xE9, 0x5C, 0x43, 0x6, 0x0, 0x0, 0xAC};
byte addr1[] = {0x28, 0x78, 0x35, 0x44, 0x6, 0x0, 0x0, 0x4E};
byte addr2[] = {0x28, 0x1D, 0x57, 0x44, 0x6, 0x0, 0x0, 0xF4};
byte addr3[] = {0x28, 0xB9, 0x2B, 0x43, 0x6, 0x0, 0x0, 0xBE};

#define NUMBER_OF_SENSORS 4
byte* addrs[NUMBER_OF_SENSORS];

float temperatures[NUMBER_OF_SENSORS];

byte sensorIndex = 0;

// lcd setup - https://learn.adafruit.com/character-lcds/rgb-backlit-lcds

#define REDLITE 3
#define GREENLITE 5
#define BLUELITE 6
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
// you can change the overall brightness by range 0 -> 255
int brightness = 255;

void setup(void) {
   // set up the LCD's number of rows and columns:
  lcd.begin(16, 2);
  
  addrs[0] = addr0;
  addrs[1] = addr1;
  addrs[2] = addr2;
  addrs[3] = addr3;

  lcd.print("KIT LIV BED OUT");

  pinMode(REDLITE, OUTPUT);
  pinMode(GREENLITE, OUTPUT);
  pinMode(BLUELITE, OUTPUT);
   
  setBacklight(255, 255, 255);
}

void loop(void) {
  // fake cooperative threading - everything checks the clock rather than calling delay()
  tickSensors();
  tickDisplay();
}

void tickDisplay() {
  const int waitLength = 200; // in ms. 200ms = 5 frames per second
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  int otherColors;
  
  if ( (currentMillis > previousMillis  && currentMillis - previousMillis > waitLength) // normal case
          || (currentMillis < previousMillis && currentMillis > waitLength) // millis() rolled over to 0 recently, wait a little extra time. Happens ~ every 4 days.
    ) { 
    for(byte i = 0; i<NUMBER_OF_SENSORS; i++) {
      // choose the column for this temp
      byte col = i*4;
      lcd.setCursor(col, 1);
      lcd.print((int)temperatures[i]);
      lcd.print(" "); // this is for when the temp drops from 100 to 99 - otherwise the screen would read 990
    }

    float diff = getInsideTemp() - getOutsideTemp();
    if (diff > 0) {
       // it's hotter inside - show red
       otherColors = map(diff, 0, 3, 255, 0);
       setBacklight(255, otherColors, otherColors);
    } else {
      otherColors = map(diff, 0, -3, 255, 0);
      setBacklight(otherColors, otherColors, 255);
    }
  }  
}

void tickSensors() {
  byte i;
  byte type_s;
  byte data[12];
  byte* addr = addrs[sensorIndex];
  float celsius, fahrenheit;
  static byte waiting = false;
  const int waitLength = 750; // in ms. 750 should be enough per documentation.
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      temperatures[sensorIndex] = -1;
      sensorIndex++; // todo: figure out how to do this better
      return;
  } 
  
  if(!waiting) {
    // the first ROM byte indicates which chip
    switch (addr[0]) {
      case 0x10:
        //lcd.print("  Chip = DS18S20");  // or old DS1820
        type_s = 1;
        break;
      case 0x28:
        //lcd.print("  Chip = DS18B20");
        type_s = 0;
        break;
      case 0x22:
        //lcd.print("  Chip = DS1822");
        type_s = 0;
        break;
      default:
        //lcd.print("Device is not a DS18x20 family device.");
        return;
    } 
  
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end
    waiting = true;
    previousMillis = currentMillis; 
  } else {
    // were waiting on the sensor. 
    if ( (currentMillis > previousMillis  && currentMillis - previousMillis > waitLength) // normal case
          || (currentMillis < previousMillis && currentMillis > waitLength) // millis() rolled over to 0 recently, wait a little extra time. Happens ~ every 4 days.
    ) {
  
      waiting = false;
      ds.reset();
      ds.select(addr);    
      ds.write(0xBE);         // Read Scratchpad
    
      for ( i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = ds.read();
      }
    
      // Convert the data to actual temperature
      // because the result is a 16 bit signed integer, it should
      // be stored to an "int16_t" type, which is always 16 bits
      // even when compiled on a 32 bit processor.
      int16_t raw = (data[1] << 8) | data[0];
      if (type_s) {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10) {
          // "count remain" gives full 12 bit resolution
          raw = (raw & 0xFFF0) + 12 - data[6];
        }
      } else {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
        //// default is 12 bit resolution, 750 ms conversion time
      }
      celsius = (float)raw / 16.0;
      fahrenheit = celsius * 1.8 + 32.0;
      
      temperatures[sensorIndex] = fahrenheit;
      
      sensorIndex++;
      if (sensorIndex >= NUMBER_OF_SENSORS) {
        sensorIndex = 0;
      }
    }
  }
}

// todo: make these adjustable to arbitrary numbers of sensors
// assumes the first three sensors are inside
float getInsideTemp() {
  return (temperatures[0] + temperatures[1] + temperatures[2])/3.0;
}
// assumes the last sensor is outside
float getOutsideTemp() {
  return temperatures[3];
}

void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest!
  //r = map(r, 0, 255, 0, 100);
  //g = map(g, 0, 255, 0, 150);
  r = map(r, 0, 255, 0, brightness);
  g = map(g, 0, 255, 0, brightness);
  b = map(b, 0, 255, 0, brightness);
  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);
  analogWrite(REDLITE, r);
  analogWrite(GREENLITE, g);
  analogWrite(BLUELITE, b);
}
