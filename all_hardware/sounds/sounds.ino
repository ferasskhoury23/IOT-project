#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

#define SAMPLE_RATE 44100 // Define the sample rate
#define I2S_NUM I2S_NUM_0 // Define the I2S port
#define bluePin 2 // Replace with the actual pin connected to the blue LED
#define greenPin 4 // Replace with the actual pin connected to the green LED
#define redPin 5 // Replace with the actual pin connected to the red LED

void KeyPressedSound();
void EmptyPasswordSound();
void CorrectPasswordSound();
void WrongPasswordSound();
void resetPasswordSound();

unsigned long lastMillis = 0;
int currentFunction = 0;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize I2S configuration
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
      .communication_format = I2S_COMM_FORMAT_STAND_MSB,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true,
      .fixed_mclk = 0
  };

  // Initialize I2S driver
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  
  // Configure I2S pins
  i2s_pin_config_t pin_config = {
      .bck_io_num = 26,   // Replace with actual BCK pin
      .ws_io_num = 25,    // Replace with actual WS pin
      .data_out_num = 22, // Replace with actual data out pin
      .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_set_pin(I2S_NUM, &pin_config);

  Serial.println("Setup complete. Starting test.");
}

void loop() {
  if (millis() - lastMillis >= 5000) {
    lastMillis = millis();

    switch (currentFunction) {
      case 0:
        Serial.println("Testing KeyPressedSound...");
        KeyPressedSound();
        break;
      case 1:
        Serial.println("Testing EmptyPasswordSound...");
        EmptyPasswordSound();
        break;
      case 2:
        Serial.println("Testing CorrectPasswordSound...");
        CorrectPasswordSound();
        break;
      case 3:
        Serial.println("Testing WrongPasswordSound...");
        WrongPasswordSound();
        break;
      case 4:
        Serial.println("Testing resetpassword...");
        resetpassword();
        break;
    }

    currentFunction = (currentFunction + 1) % 5;
  }
}

//sound functions

void KeyPressedSound() //sound when any key is pressed
{
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];

   // Generate a 1kHz sine wave
  for (int i = 0; i < 64; i++) {
         samples[i] = (int16_t)(32767 * sin(phase));
         phase += 2 * PI * 5000 / SAMPLE_RATE;
         if (phase >= 2 * PI) {
            phase -= 2 * PI;
        }
      }
      int j = 0;
      while(j<200){
        i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
        j++;
      }
}


void EmptyPasswordSound() //when input is empty and user presses #
{
  Serial.println("empty password");
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];
  
   // Generate a cossine wave
  for (int i = 0; i < 64; i++) {
         samples[i] = (int16_t)(10000000 * cos(phase));
         phase += 2 * PI * 5000 / SAMPLE_RATE;
         if (phase >= 2 * PI) {
            phase -= 2 * PI;
        }
      }
      int j = 0;
      while(j<1000){
        i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
        j++;
      }
}

void CorrectPasswordSound() //sound when password is correct
{
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];
   // Generate a 1kHz sine wave
  for (int i = 0; i < 64; i++) {
         samples[i] = (int16_t)(32767 * sin(phase));
         phase += 2 * PI * 5000 / SAMPLE_RATE;
         if (phase >= 2 * PI) {
            phase -= 2 * PI;
        }
      }
      int j = 0;
      while(j<1900){
        i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
        j++;
      }
}

void WrongPasswordSound() //sound when password is incorrect
{
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];
   // Generate a 1kHz sine wave
  for (int i = 0; i < 64; i++) {
         samples[i] = (int16_t)(32767 * sin(phase));
         phase += 2 * PI * 5000 / SAMPLE_RATE;
         if (phase >= 2 * PI) {
            phase -= 2 * PI;
        }
      }
      // make beeping sound 5 times
      for (int k = 0; k < 5; k++) {
          int j=0;
          while(j<200){
             i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
            j++;
          }
          delay(300);
        }
      
    }


    //lights flickering when * is pressed to reset the password
    void resetPasswordSound()
    {

      //generate first sine wave
      static float phase1 = 0;
      size_t bytes_written1;
      int16_t samples1[64];
      // Generate a 1kHz sine wave
      for (int i = 0; i < 64; i++) {
         samples1[i] = (int16_t)(32767 * sin(phase1));
         phase1 += 2 * PI * 5000 / SAMPLE_RATE;
         if (phase1 >= 2 * PI) {
            phase1 -= 2 * PI;
        }
      }


      //generate second cosine wave
      static float phase2 = 0;
      size_t bytes_written2;
      int16_t samples2[64];
      // Generate a xkHz sine wave
      for (int i = 0; i < 64; i++) {
         samples2[i] = (int16_t)(100000 * cos(phase2));
         phase2 += 2 * PI * 5000 / SAMPLE_RATE;
         if (phase2 >= 2 * PI) {
            phase2 -= 2 * PI;
        }
      }


      // make beeping sound 3 times
      for (int k = 0; k < 3; k++) {
          if(k%2 == 0)
          {
            int j1=0;
        
            while(j1<100){
              i2s_write(I2S_NUM, samples1, sizeof(samples1), &bytes_written1, portMAX_DELAY);
              j1++;
            }
            delay(50);
            }
          else
          {
            int j2=0;
            while(j2<100){
              i2s_write(I2S_NUM, samples2, sizeof(samples2), &bytes_written2, portMAX_DELAY);
              j2++;
            }
            delay(50);
          }
        }
        Serial.println("password has been reset");
    }


