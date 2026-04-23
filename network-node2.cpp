#include "network.h"

static PubSubClient* mqtt       =NULL;
static byte          mac[6]     = NODE_MAC;
static unsigned long lastReconnectTime = 0;


static void doConnect(void)
{
  //connecting to things board
  if(mqtt->connect(CLIENT_ID, ACCESS_TOKEN, NULL))
  {
    Serial.println("connected");
    mqtt->subscribe(TOPIC_RPC_RESPONSE);
  }
  else
  {
    Serial.println("mqtt failed");
  }
}


static void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message received: ");

  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void network_begin(PubSubClient* mqttClient)
{
  mqtt   =mqttClient;
  Ethernet.begin(mac);
  delay(2000);

  //connect to thingsboard
  mqtt->setServer(TB_SERVER, TB_PORT);
  mqtt->setCallback(mqttCallback);
  doConnect();
  
}

void    network_maintain(void)
{
  Ethernet.maintain();

  if(mqtt->connected())
  {
    mqtt->loop();
    
  }
  else
  {
    unsigned long now = millis();
    if(now - lastReconnectTime >= RECONNECT_INTERVAL)
    {
      lastReconnectTime = now;
      doConnect();
    }
  }
}


//return the status of mqtt conection
uint8_t network_isConnected(void)
{
  return (uint8_t ) mqtt->connected();
  
}


uint8_t network_publish(const char* topic, const char* payload, uint8_t retain)
{
  return (uint8_t) mqtt->publish(topic, payload, retain);
}
