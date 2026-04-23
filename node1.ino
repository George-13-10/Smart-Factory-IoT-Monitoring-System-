//include required libraries
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "actuators.h"

#include "config.h"
#include "SensorManager.h"
#include "network.h"
#include "telemetry.h"
#include "SensorManager.h"
#include "rpc.h"

static char sharedBuf[200];

//creat object of type ethernet pubsubclient
static EthernetClient ethClient;
static PubSubClient mqttClient(ethClient);

unsigned long lastTelemetry = 0;



void setup() 
{
  
  Serial.begin(9600);
  Serial.print("Node1 is booting......");

  // put your setup code here, to run once:
   // initilase sensor 
  sensor_begin();
   //initialise actutors
  actuators_begin();


  telemetry_init(sharedBuf,sizeof(sharedBuf));
  rpc_init(&mqttClient,sharedBuf, sizeof(sharedBuf));//initialising rpc request
   
  mqttClient.setCallback(rpc_mqttCallback);

   // coonect board to internet and mqtt
   
  network_begin(&mqttClient);
  mqttClient.setCallback(rpc_mqttCallback);


  digitalWrite(PIN_LED_RED,LOW);
   

}

void loop() 
{
  // put your main code here, to run repeatedly:
  //keep checking the board is connected to things board

  network_maintain();
  

  
  //read data from the sensors
  SensorData data;
  sensors_read( &data);


   
  //publish to the cloud
  //check if network available then push data to TB
  if(network_isConnected())
  {
    unsigned long now = millis();
    //publish data to the things every 5sec
    if(now - lastTelemetry >= TELEMETRY_INTERVAL)
    {
      lastTelemetry = now ;
      telemetry_publishTelemetry(&data,actuators_getRelayState()); //cobverting sensor data into json and publish
      
      
    }
  }
  
  actuators_updateStatusLEDs(network_isConnected(),data.sensorError);


}
