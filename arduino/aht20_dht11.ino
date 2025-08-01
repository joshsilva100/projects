/* aht20_dht11.ino
 * Task 1 - Embedded Systems & Sensors 
 * HVAC/Smart Building
 * Joshua Silva & Richard Pressley
 *
 * Purpose: Initialize and collect temp/humidity data from the AHT20 and DHT11 Sensors 
 * 
 * Assumptions: User has a completed circuit and uses the same pins as described in the sketch 
 *              User knows that the DHT11 sensor a refresh rate of 1 Hz (reality is every 2 seconds gives a new reading)
 *
 * Bugs: Disrupting running of one sensor can impact the other. For example, unplugging the DHT11 can give the AHT20 a reading of -50.00, 0.00
 * 
 * Output format is AHT_Temp,AHT_Humidity,DHT_Temp,DHT_Humidity
 */ 

#include <Adafruit_AHTX0.h> 
#include <DHT.h>

/* Initialize AHT Sensor */ 
// SCL to A5 & SDA to A4 (Uno defaults for serial clk and serial data respectively)
Adafruit_AHTX0 aht; 

/* Initialize DHT sensor */ 
#define DHTPIN 2 // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT11 // Using the DHT11 sensor
DHT dht(DHTPIN, DHTTYPE); 

// Initialize previous values in case of sensor failure
float prev_a_temp = 0.00; 
float prev_a_humidity = 0.00; 
float prev_d_temp = 0.00;
float prev_d_humidity = 0.00;

/* Setup both sensors */
void setup() {
  Serial.begin(9600);

  if (!aht.begin()) {
    Serial.println("Could not find AHT, please check wiring");
    while (1) delay(10);
  } 

  dht.begin(); // Cannot error check DHT11 setup, it returns a void. Check for later error messages or a constant 0.0 in Serial Monitor
}

/* Read data from both sensors every second */
void loop() {
  
  /* AHT Humidity and Temp */
  sensors_event_t a_humidity, a_temp;
  aht.getEvent(&a_humidity, &a_temp); // populate temp and humidity objects with fresh data 

  // If AHT20 sensor fails (rare), report previous valid data 
  if (isnan(a_temp.temperature) || (a_temp.temperature <= -50.00)) a_temp.temperature = prev_a_temp; 
  else prev_a_temp = a_temp.temperature; 

  if (isnan(a_humidity.relative_humidity) || (a_humidity.relative_humidity <= 0.00)) a_humidity.relative_humidity = prev_a_humidity; 
  else prev_a_humidity = a_humidity.relative_humidity;

  /* DHT Humidity and Temp */
  float d_humidity = dht.readHumidity();
  float d_temp = dht.readTemperature(); //Reads in Celsius(default) 

  // If DHT11 sensor fails (rare), report previous valid data. Would like to find a good temperature that indicates failure, usually 0.3 to 0.5 is failure but that's about freezing
  if (isnan(d_temp) || d_temp <= 0.50) d_temp = prev_d_temp; 
  else prev_d_temp = d_temp; 

  if (isnan(d_humidity) || d_humidity <= 0.00) d_humidity = prev_d_humidity; 
  else prev_d_humidity = d_humidity;

  /* Print out Sensor Data */ 

  // AHT Sensor Output
  Serial.print(a_temp.temperature); Serial.print(F(","));
  Serial.print(a_humidity.relative_humidity); Serial.print(F(","));

  // DHT Sensor Output
  Serial.print(d_temp); Serial.print(F(","));
  Serial.print(d_humidity);
  
  Serial.println();
  delay(1000);
}