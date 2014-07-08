/* ATtiny85 as an I2C Master   Ex2        BroHogan                           1/21/11
 * Modified for Digistump - Digispark LCD Shield by Erik Kettenburg 11/2012
 * SETUP:
 * ATtiny Pin 1 = (RESET) N/U                      ATtiny Pin 2 = (D3) N/U
 * ATtiny Pin 3 = (D4) to LED1                     ATtiny Pin 4 = GND
 * ATtiny Pin 5 = SDA on DS1621  & GPIO            ATtiny Pin 6 = (D1) to LED2
 * ATtiny Pin 7 = SCK on DS1621  & GPIO            ATtiny Pin 8 = VCC (2.7-5.5V)
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * PCA8574A GPIO was used wired per instructions in "info" folder in the LiquidCrystal_I2C lib.
 * This ex assumes A0-A2 are set HIGH for an addeess of 0x3F
 * LiquidCrystal_I2C lib was modified for ATtiny - on Playground with TinyWireM lib.
 * TinyWireM USAGE & CREDITS: - see TinyWireM.h
 */

//#define DEBUG
#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI - comment this out to use with standard arduinos
#include <LiquidCrystal_I2C.h>          // for LCD w/ GPIO MODIFIED for the ATtiny85

#define GPIO_ADDR     0x27             // (PCA8574A A0-A2 @5V) typ. A0-A3 Gnd 0x20 / 0x38 for A - 0x27 is the address of the Digispark LCD modules.


LiquidCrystal_I2C lcd(GPIO_ADDR,16,2);  // set address & 16 chars / 2 lines


void setup(){
  TinyWireM.begin();                    // initialize I2C lib - comment this out to use with standard arduinos
  lcd.init();                           // initialize the lcd (defaults to pins 0 & 2)
  lcd.backlight();                      // light up the LCD
  //lcd.setCursor(5, 1); // fifth character second column
  //lcd.print((char)223); // degree symbol
  //lcd.print("F");
  pinMode(4, INPUT); // Digital 4 is analog (ADC channel) 2
  pinMode(5, OUTPUT);
  lcd.print("Cutoff Temp ");
}

int sensorValue = 0;

void loop(){
  lcd.setCursor(12, 0);
  sensorValue = analogRead(2); // Digital 4 is analog 2
  lcd.print(doubleMap(sensorValue, 0, 1023, 60, 80.1)); // it stops at 79.9 when I set the max to 80
  delay(250);
}

// reads the internal tempature of the CPU's internal thermomiter
int get_internal_temp() {
  analogReference(INTERNAL1V1);
  int raw = analogRead(A0+15); 
  int in_c = raw - 273; // celcius
  analogReference(DEFAULT);
  return in_c;
}

float c_to_f(int celsius)
{
  return 1.8 * celsius + 32;
}

// based on http://www.arduino.cc/en/Reference/Map but with doubles
double doubleMap(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
