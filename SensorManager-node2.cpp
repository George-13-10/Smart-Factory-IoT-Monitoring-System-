#include "SensorManager.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT_TYPE);

void Sensor_init  (SensorManager* sm)
{
  sm->lastTemp =0.0f;
  sm->lastHumidity =0.0f;
  sm->motionActive = false;
  sm->motionClearTime = 0;
  
}


void Sensor_begin(void)
{
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_DOOR, INPUT_PULLUP);
  dht.begin();
}

//READ SENSOR OF pir
static bool readPIR(SensorManager* sm)
{
  //read the value from motion sens,check if it is true for next 10 secs
  const bool pirHigh = (digitalRead(PIN_PIR) == HIGH);
  const unsigned long now = millis();
  if(pirHigh)
  {
    sm->motionActive = true;
    sm->motionClearTime = now + MOTION_CLEAR_MS;
    
  }
  else if(sm->motionActive && (now > sm->motionClearTime))
  {
    sm->motionActive = false;
  }
  return sm->motionActive;
  
}


//READ DOOR STATUS 
static bool readDoor(void)
{
  return (digitalRead(PIN_DOOR)==LOW);//0 door is open ,1 door is closed
}

//read ldr sensor
static uint16_t readLDR(void)
{
  uint16_t sum =0;
  uint8_t i;
  for(i=0;i<4;i++)
  {
    sum += (uint16_t)analogRead(PIN_LDR);
    delay(2);
  }
  return sum/4;//ware house is dark or not ,300>val->dark,light is avilable
  
}

//read data from the sensor ,return status of DHT22


bool Sensor_read(SensorData* out, SensorManager* sm)
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // check DHT error
  if (isnan(t) || isnan(h))
  {
    out->sensorError = true;
    out->temperature = sm->lastTemp;
    out->humidity = sm->lastHumidity;
  }
  else
  {
    out->sensorError = 0;
    out->temperature = round(t * 10.0f) * 0.1f;
    out->humidity = round(h * 10.0f) * 0.1f;

    sm->lastTemp = out->temperature;
    sm->lastHumidity = out->humidity;
  }
  //READ LDR ,DOOR STATUS,MOTION
  out->motionDetected = readPIR(sm);
  
  out->doorOpen = readDoor();

  out->ldrValue = readLDR();
  out->isDark = (out->ldrValue<LDR_DARK_THRESH);

  return !out->sensorError;
}
