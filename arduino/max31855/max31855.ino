/* MAX31855
 * Joshua Silva
 *
 * Purpose: Get temperature data from the MAX31855 Thermocouple Amp. Makes use of the Adafruit_MAX31855 example sketch 
 * 
 * Assumptions: User knows that the output is in Celsius
 * 
 * Output format is: Internal_temp, External_temp 
 */  

#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Thermocouple instance with software SPI. See digital pins below
#define MAXCLK 3
#define MAXCS 4
#define MAXDO 5

// Initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc
}

void loop() {
  // Basic readout test, just print the current temp
   Serial.print(thermocouple.readInternal()); 
   Serial.print(F(","));

   double c = thermocouple.readCelsius();
   if (isnan(c)) 
   {
     Serial.println("Thermocouple fault(s) detected!");
     uint8_t e = thermocouple.readError();
     if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
     if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
     if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
   } 
   
   else 
   {
     Serial.print(c); 
     Serial.print("\n");
   }

   delay(1000);
}
