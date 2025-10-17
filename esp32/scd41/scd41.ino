/* scd41.ino
 * HVAC/Smart Building
 * Joshua Silva
 *
 * Purpose: Initialize and collect CO2 data from the Adafruit SCD-41 CO2 sensor 
 *          Don't need the extra Temperature and Humidity data
 * 
 * Output format is CO2_Concentration
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
    //scd41.startPeriodicMeasurement();
    scd41.startLowPowerPeriodicMeasurement(); // Use low power in preparation of complete prototype
}

void loop() {
    uint16_t co2_ppm = 0;
    float temp = 0.0;
    float humid = 0.0;

    // Raw works fine as well if we don't care about extra checks & float conversion
    scd41.readMeasurement(co2_ppm, temp, humid); 
    
    Serial.print(co2_ppm);
    // Serial.print(F(","));
    // Serial.print(temp);
    // Serial.print(F(","));
    // Serial.print(humid);
    Serial.print(F("\n")); 

    //delay(5000); 
    delay(30000); // With low power, takes 25 seconds longer to get next reading
}
