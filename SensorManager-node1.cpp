#include "SensorManager.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT_TYPE);

static float lastTemp;
static float lastHumidity; 

void sensor_begin(void)
{
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  dht.begin();
}

// read LM35
static float readLM35(void)
{
  // read temperature from LM35 convert to celsius
  float t = analogRead(PIN_LM35) * (500.0f / 1023.0f);
  return round(t * 10.0f) * 0.1f;
}

// read vibrations from POT
static uint16_t readVibration(void)
{
  uint16_t sum = 0;
  uint8_t i;

  for(i = 0; i < 4; i++)
  {
    sum += analogRead(PIN_VIBRATION);
    delay(1000);
  }
  return sum / 4;
}

// read data from sensors
// return 0 on error
uint8_t sensors_read(SensorData* out)
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // check DHT error
  if (isnan(t) || isnan(h))
  {
    out->sensorError = 1;
    out->temperature = lastTemp;
    out->humidity = lastHumidity;
  }
  else
  {
    out->sensorError = 0;
    out->temperature = round(t * 10.0f) * 0.1f;
    out->humidity = round(h * 10.0f) * 0.1f;

    lastTemp = out->temperature;
    lastHumidity = out->humidity;
  }

  // read other sensors
  out->lm35Temp = readLM35();
  out->vibration = readVibration();
  out->machineStatus = (digitalRead(PIN_BUTTON) == LOW) ? 1 : 0;

  return !out->sensorError;
}
