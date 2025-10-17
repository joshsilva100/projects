/* scd41.ino
 * HVAC/Smart Building
 * Joshua Silva
 *
 * Purpose: Initialize and collect CO2 data, Temperature, and Humidity from the Adafruit SCD-41 CO2 sensor
 * 
 * Output format is CO2_Concentration,Temperature,Humidity 
 * 
 * More info: https://github.com/Sensirion/arduino-i2c-scd4x/tree/master/src
 */ 

#include <SensirionI2cScd4x.h>
#include <Wire.h>

SensirionI2cScd4x scd41;

void setup() {

    Serial.begin(115200);
    Wire.begin();
    scd41.begin(Wire, SCD41_I2C_ADDR_62);
    scd41.wakeUp();
    scd41.startPeriodicMeasurement();
}

void loop() {
    uint16_t co2_ppm = 0;
    float temp = 0.0;
    float humid = 0.0;
    
    scd41.readMeasurement(co2_ppm, temp, humid);
    
    Serial.print(co2_ppm);
    Serial.print(F(","));
    Serial.print(temp);
    Serial.print(F(","));
    Serial.print(humid);
    Serial.print(F("\n")); 

    delay(5000);
}
