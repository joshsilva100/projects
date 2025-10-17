/* veml7700.ino
 * HVAC/Smart Building
 * Joshua Silva
 *
 * Purpose: Initialize and collect LUX data from the VEML-7700 sensor
 * 
 * Output format is Lux
 * 
 * More info: https://github.com/adafruit/Adafruit_VEML7700
 */ 

#include "Adafruit_VEML7700.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();

void setup() {
  Serial.begin(115200); 

  veml.begin();

  // == OPTIONAL =====
  // Can set non-default gain and integration time to
  // adjust for different lighting conditions.
  // =================
  // veml.setGain(VEML7700_GAIN_1_8);
  // veml.setIntegrationTime(VEML7700_IT_100MS);

  // Don't need interrupts!
  // veml.setLowThreshold(10000);
  // veml.setHighThreshold(20000);
  // veml.interruptEnable(true);
}

void loop() {
  // Serial.print(veml.readALS()); 
  // Serial.print(F(","));
  // Serial.print(veml.readWhite()); 
  // Serial.print(F(","));
  Serial.print(veml.readLux()); 
  Serial.print(F("\n"));

  // Don't need interupts!
  // uint16_t irq = veml.interruptStatus();
  // if (irq & VEML7700_INTERRUPT_LOW) {
  //   Serial.println("** Low threshold");
  // }
  // if (irq & VEML7700_INTERRUPT_HIGH) {
  //   Serial.println("** High threshold");
  // }
  delay(500);
}