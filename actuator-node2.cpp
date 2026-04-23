#include "actuator.h"

static uint8_t relay_status;
static unsigned long lastBlinkTime = 0;
static uint8_t blinkstate = 0;

void actuators_begin(void)
{
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN , OUTPUT);//ON everything is normal
  pinMode(PIN_LED_YELLOW , OUTPUT);//on-.WARNING ,BLINKING-.critical
  pinMode(PIN_RELAY, OUTPUT);//ventilation ,if more humidity

// in the beginning till network connection LED be on
  digitalWrite(PIN_LED_GREEN, LOW); 
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_RELAY, LOW);
}


void    actuators_setRelay(uint8_t on)
{
  relay_status = on;
  digitalWrite(PIN_RELAY,on ?HIGH: LOW);
  
}

//send back the status of the relay to thingsboard
uint8_t actuators_getRelayState(void)
{
  
  return relay_status;
}


void actuators_updateStatusLEDs(uint8_t mqttConnected, uint8_t sensorError)
{
 // board not connected to thingsboard then turn on red led
  if (!mqttConnected)   // ✅ 
  {
    digitalWrite(PIN_LED_RED,  HIGH);
    digitalWrite(PIN_LED_GREEN , LOW);
    digitalWrite(PIN_LED_YELLOW , LOW);
    digitalWrite(PIN_LED_RED , HIGH);
    return;
  }

  digitalWrite(PIN_LED_RED , LOW);

  // if sensor error blink yellow led
  if (sensorError)
  {
    digitalWrite(PIN_LED_GREEN , LOW);
    unsigned long now = millis();

    if (now - lastBlinkTime > 500)  
    {
      lastBlinkTime = now;
      blinkstate = !blinkstate;
      digitalWrite(PIN_LED_YELLOW, blinkstate ? HIGH : LOW); 
    }
  }
  else
  {
    // all good
    digitalWrite(PIN_LED_GREEN , HIGH);
    digitalWrite(PIN_LED_YELLOW , LOW);
  }
}
