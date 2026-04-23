//include required libraries
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "config.h"
#include "SensorManager.h"
#include "actuator.h"
#include "network.h"
#include "telemetry.h"
#include "rpc.h"

static char sharedBuf[200];

//creat object of type ethernet pubsubclient
static EthernetClient ethClient;
static PubSubClient mqttClient(ethClient);


unsigned long lastTelemetry = 0;

static SensorManager   sensors;




void setup() 
{
  Serial.begin(9600);
  Serial.print("Node2 is booting...");

  // put your setup code here, to run once:
   // initilase sensor 
   Sensor_begin();
   Sensor_init(&sensors);
   
   //initialise actutors
   actuators_begin();
   
   // coonect board to internet and mqtt

   telemetry_init(sharedBuf, sizeof(sharedBuf));
   rpc_init(&mqttClient,sharedBuf, sizeof(sharedBuf));

   network_begin(&mqttClient);

   

}

void loop() 
{
  // put your main code here, to run repeatedly:
  //read data from the sensors 
  //publish to the cloud 

  network_maintain();
  

  
  //read data from the sensors
  SensorData data;
  Sensor_read( &data,&sensors);//humidity ,temp,ldr,door status,motion status


  if(data.humidity >=HUMIDITY_CRIT && !actuators_getRelayState()) 
  {
    actuators_setRelay(1);
    
  }

  
   
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
