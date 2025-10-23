/* i2s_mic.ino
 * HVAC/Smart Building
 * Joshua Silva
 *
 * Purpose: Initialize and collect noise data from the I2S MEMS Microphone (SPH0645)
 * 
 * Output format is Noise 
 * 
 * More info: https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2s.html#sample-code 
              https://www.youtube.com/watch?v=AGeIdqEWdR0 
              https://cdn-learn.adafruit.com/assets/assets/000/123/406/original/adafruit_products_Adafruit_ESP32_Feather_V2_Pinout.png?1691707257
              "The I2S is a small, low-cost MEMS mic with a range of about 50Hz - 15KHz"
 */ 

#include <ESP_I2S.h>

// I2S pins on ESP32 Feather V2
#define I2S_LRC 25 // A1/DAC1
#define I2S_DIN 26 // DOUT from mic A0/DAC2
#define I2S_BCLK 5 // SCK

const int sampleRate = 15000;  // sample rate in Hz 

i2s_mode_t mode = I2S_MODE_STD; 
i2s_data_bit_width_t bps = I2S_DATA_BIT_WIDTH_32BIT;
i2s_slot_mode_t slot = I2S_SLOT_MODE_MONO;

I2SClass i2s;

void setup() {
  Serial.begin(115200);

  // Use suggested pins. We do not need to write data to the sensor, give bogus value
  i2s.setPins(I2S_BCLK, I2S_LRC, -1, I2S_DIN);

  // start I2S at the sample rate with 32-bits per sample
  i2s.begin(mode, sampleRate, bps, slot); 
}

void loop() {
  int noise = 0;
  noise = i2s.read(); 
  Serial.print(noise); 
  Serial.print(F("\n")); 
  //delay(10);
} 
