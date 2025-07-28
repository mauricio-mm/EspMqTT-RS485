#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "FS.h"
#include "SPIFFS.h"
#include "utils.h"

WiFiClient espClient;
PubSubClient MQTT(espClient);
DHT dht(DHTPIN, DHTTYPE);
Servo servo;

static long long tempo=0;

const char* mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;

void setup() 
{  
    Serial.begin(9600);
    WIFIConnect(&espClient);
    MQTT.setServer(mqtt_broker, mqtt_port);   
    MQTT.setCallback(callback);

    //LED CONF
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, HIGH);

    //DHT CONF
    dht.begin();
    pinMode(DHTPIN, INPUT);

    //SERVO CONF
    servo.attach(SERVOPIN);
} 

void loop() 
{
  static long long pooling = 0;
  
  if(!MQTT.connected()) MQTTConnect(&MQTT);
  if(WiFi.status() != WL_CONNECTED) WIFIConnect(&espClient);
  
  if(millis() > pooling+5000)
  {
    pooling = millis();
    publish_data(&MQTT, topic_dht, String(dht.readTemperature()) + ':' +String(dht.readHumidity()));
  }  

  MQTT.loop();
}