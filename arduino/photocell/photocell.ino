/*
  PhotoCell
  Joshua Silva 

  Purpose: Use photocells to detect lighting changes. 
           Read the analog output value(0-1023) for 5Vin of the pulldown resistor and print it to the serial monitor. 

  Note: Less light, lower voltage (lower Analog reading). Expect 'darkRoom' to have a higher reading, 
        not meant to determine differences between 'bright' and 'really bright.' Use pulldown resistors 
        to implement voltage divider, measure voltage across pulldown resistors.

  Output Format is: brightRoom_light,brightRoom_voltage, darkRoom_light,darkRoom_voltage
*/ 

#define brightRoom A1 // Analog1 input w/ 1K pulldown resistor
#define darkRoom A2 // Analog2 input w/ 10K pulldown resistor

void setup() {
  Serial.begin(9600);
  
  // Just as a precaution
  pinMode(brightRoom, INPUT); 
  pinMode(darkRoom, INPUT); 
}

void loop() {
  // Handling output of the photocells. Testing resistors values for both intended for differing default lightings
  Serial.print(analogRead(brightRoom)); 
  Serial.print(F(",")); 
  Serial.print(analogRead(brightRoom) * (5.0 / 1023.0)); 
  Serial.print(F(","));
  Serial.print(analogRead(darkRoom)); 
  Serial.print(F(",")); 
  Serial.print(analogRead(darkRoom) * (5.0 / 1023.0)); 
  Serial.print("\n"); 

  delay(1000);
}
