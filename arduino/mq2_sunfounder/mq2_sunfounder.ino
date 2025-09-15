/*
  mq2_sunfounder 
  Joshua Silva 

  Purpose: Read the output values of the gas sensor and print it to the serial monitor. 

  Note: After the sensor is powered on, it needs to be preheated for 
  at least 3 minute before stable measurement readings can be obtained. 
  It is normal for the sensor to generate heat during operation due to 
  the presence of a heating wire inside. 

  Sketch is adjusted from Sunfounder's Guide on the MQ2 Gas Sensor. https://docs.sunfounder.com/projects/umsk/en/latest/02_arduino/uno_lesson04_mq2.html 

  Output Format is: AnalogGas,AnalogGas_Volt,DigitalGas
*/

// Gas Sensor Pins
#define analogGas A0 
#define digitalGas 7 

void setup() {
  Serial.begin(9600);  
  pinMode(analogGas, INPUT);
  pinMode(digitalGas, INPUT);
}

void loop() {
  //Print out both Analog and Digital outputs. For comparison later
  Serial.print(analogRead(analogGas)); 
  Serial.print(F(","));  
  Serial.print(analogRead(analogGas) * (5.0 / 1023.0)); 
  Serial.print(F(","));
  Serial.print(digitalRead(digitalGas)); 
  Serial.print("\n"); 

  delay(1000);
}