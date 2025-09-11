/* ky038
 * Joshua Silva 
 * 
 * Purpose: To collect sound data from the Microphone Sound Sensor Module KY-038
 * 
 * Notes: Turn potentiometer to change sensitivity 
 * 
 * Output format is: analog_out,digital_out
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
  Serial.print(digitalRead(digital)); 
  Serial.print("\n");

  delay(1000); 
}
