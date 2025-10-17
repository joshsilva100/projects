/* hdc3022.ino
 * HVAC/Smart Building
 * Joshua Silva
 *
 * Purpose: Initialize and collect temp/humidity data from the Adafruit HDC3022 sensor
 * 
 * Output format is Temperature,Humidity 
 * 
 * More info: https://github.com/adafruit/Adafruit_HDC302x/tree/main
 */ 

//Same as HDC3021
#include <Adafruit_HDC302x.h>

Adafruit_HDC302x hdc = Adafruit_HDC302x();

void setup() {
  Serial.begin(115200); 
  hdc.begin(0x44, &Wire); //Default I2C address
}

void loop() {
  //Initialize, potential sensor error if Serial Monitor reads zero
  double temp = 0.0;
  double humid = 0.0;

  //LP0 Lowest Noise. Goes up to LP3, go for low power use
  hdc.readTemperatureHumidityOnDemand(temp, humid, TRIGGERMODE_LP3); 

  Serial.print(temp); //In Celsius by default 
  Serial.print(F(","));
  Serial.print(humid); 
  Serial.print(F("\n"));
  delay(1000); //Can read data every second
  
}
