#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

// Constants
#define SAMPLE_RATE 44100
#define I2S_NUM I2S_NUM_0
#define AMPLITUDE 2000 // controls the volume

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// vin -> 3.3V
// GND -> GND
// BCLK ->  2
// LRCLK ->  4
// DIN -> 15
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function Declarations
void setupI2S();
void KeyPressedSound();
void EmptyPasswordSound();
void CorrectPasswordSound();
void WrongPasswordSound();
void resetPasswordSound();
void DoorbellRingSound();


// I2S setup function
void setupI2S() {
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

  i2s_pin_config_t pin_config = {
      .bck_io_num = 2,
      .ws_io_num = 4,
      .data_out_num = 15,
      .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
}

// Sound functions implementation
void KeyPressedSound() {
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];

  for (int i = 0; i < 64; i++) {
    samples[i] = (int16_t)(AMPLITUDE * sin(phase)); // was 32767
    phase += 2 * PI * 5000 / SAMPLE_RATE;
    if (phase >= 2 * PI) phase -= 2 * PI;
  }

  int j = 0;
  while (j < 200) {
    i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
    j++;
  }
}

void EmptyPasswordSound() {
  Serial.println("empty password");
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];

  for (int i = 0; i < 64; i++) {
    samples[i] = (int16_t)(AMPLITUDE * cos(phase)); // was 10000000
    phase += 2 * PI * 5000 / SAMPLE_RATE;
    if (phase >= 2 * PI) phase -= 2 * PI;
  }

  int j = 0;
  while (j < 1000) {
    i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
    j++;
  }
}

void CorrectPasswordSound() {
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];

  for (int i = 0; i < 64; i++) {
    samples[i] = (int16_t)(AMPLITUDE * sin(phase)); // was 32767
    phase += 2 * PI * 5000 / SAMPLE_RATE;
    if (phase >= 2 * PI) phase -= 2 * PI;
  }

  int j = 0;
  while (j < 1900) {
    i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
    j++;
  }
}

void WrongPasswordSound() {
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];

  for (int i = 0; i < 64; i++) {
    samples[i] = (int16_t)(AMPLITUDE * sin(phase)); // was 32767
    phase += 2 * PI * 5000 / SAMPLE_RATE;
    if (phase >= 2 * PI) phase -= 2 * PI;
  }

  for (int k = 0; k < 5; k++) {
    int j = 0;
    while (j < 200) {
      i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
      j++;
    }
    delay(300);
  }
}

void resetPasswordSound() {
  static float phase1 = 0, phase2 = 0;
  size_t bytes_written1, bytes_written2;
  int16_t samples1[64], samples2[64];

  for (int i = 0; i < 64; i++) {
    samples1[i] = (int16_t)(AMPLITUDE * sin(phase1));// was 32767
    phase1 += 2 * PI * 5000 / SAMPLE_RATE;
    if (phase1 >= 2 * PI) phase1 -= 2 * PI;

    samples2[i] = (int16_t)(AMPLITUDE * cos(phase2)); // was 100000
    phase2 += 2 * PI * 5000 / SAMPLE_RATE;
    if (phase2 >= 2 * PI) phase2 -= 2 * PI;
  }

  for (int k = 0; k < 3; k++) {
    int j1 = 0;
    while (j1 < 100) {
      i2s_write(I2S_NUM, samples1, sizeof(samples1), &bytes_written1, portMAX_DELAY);
      j1++;
    }
    delay(50);

    int j2 = 0;
    while (j2 < 100) {
      i2s_write(I2S_NUM, samples2, sizeof(samples2), &bytes_written2, portMAX_DELAY);
      j2++;
    }
    delay(50);
  }
}


void DoorbellRingSound() {
  static float phase = 0;
  size_t bytes_written;
  int16_t samples[64];

  // Generate a 2kHz sine wave for the doorbell
  for (int i = 0; i < 64; i++) {
    samples[i] = (int16_t)(AMPLITUDE * sin(phase)); // was 32767
    phase += 2 * PI * 2000 / SAMPLE_RATE; // 2kHz frequency
    if (phase >= 2 * PI) {
      phase -= 2 * PI;
    }
  }

  // Play the sound for a short duration
  int j = 0;
  while (j < 300) { // Adjust the number of iterations for desired duration
    i2s_write(I2S_NUM, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
    j++;
  }
}
