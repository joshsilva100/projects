/* ky038
 * Joshua Silva 
 * 
 * Purpose: To collect sound data from the Microphone Sound Sensor Module KY-038
 * 
 * Notes: Turn potentiometer to change sensitivity. Left for less sensitivity, and vice versa. 
          digital_out should be 1 when noise detected out 
          analog values lower with more noise
 * 
 * Output format is: analog_out, analog_voltage, digital_out
 */

#define analog A3 
#define digital 8

void setup() {
  Serial.begin(9600); 

  // Just in case
  pinMode(analog, INPUT); 
  pinMode(digital, INPUT);
}

void loop() { 
  // Print out analog and digital readings from the KY-038
  Serial.print(analogRead(analog)); 
  Serial.print(F(",")); 
  Serial.print(analogRead(analog) * (5.0 / 1023.0));
  Serial.print(F(",")); 
  Serial.print(digitalRead(digital)); 
  Serial.print("\n");

  delay(1000); 
}
