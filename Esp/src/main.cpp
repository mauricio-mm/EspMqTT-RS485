#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include "FS.h"
#include "SPIFFS.h"
#include "utils.h"

#define MAX485_DE_RE 4

WiFiClient espClient;
PubSubClient MQTT(espClient);

static long long tempo=0;

const char* mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;

// half second wait for a reply
int TIMEOUT = 1500;

// Dado a ser transmitido
unsigned char msg_tx[8] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00};
unsigned char msg_sem_crc[6] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x02};
unsigned char crc_saida[2];

// Dado a ser recebido
unsigned char msg_rx[12];

void setup() 
{      
    Serial.begin(115200);
    WIFIConnect(&espClient);
    MQTT.setServer(mqtt_broker, mqtt_port);   
    MQTT.setCallback(callback);

    calculateCRC(msg_sem_crc, 6, crc_saida);
    msg_tx[6] = crc_saida[0];
    msg_tx[7] = crc_saida[1];

    Serial1.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(MAX485_DE_RE, OUTPUT);
    digitalWrite(MAX485_DE_RE, LOW);
    delay(1000);

} 

void loop() 
{
  static long long pooling = 0;
  
  if(!MQTT.connected()) MQTTConnect(&MQTT);
  if(WiFi.status() != WL_CONNECTED) WIFIConnect(&espClient);

  uint32_t startTime = 0;
  int j = 0;

  Serial.print("TX: ");
  printHexMessage(msg_tx, sizeof(msg_tx) );

  digitalWrite(MAX485_DE_RE, HIGH);
  delay(2);

  Serial1.write(msg_tx, sizeof(msg_tx) );
  Serial1.flush();

  digitalWrite(MAX485_DE_RE, LOW);

  Serial.print("RX: ");
  
  startTime = millis();

    while ( millis() - startTime <= TIMEOUT ) {
      if (Serial1.available()) {
        msg_rx[j] = Serial1.read();
        j++;
      }
    } 

    for(j=0; j<10; j++)
      printHexByte(msg_rx[j]);

    if(millis() > pooling+5000)
    {
    if (j >= 7) {
        uint16_t high = (msg_rx[3] << 8) | msg_rx[4];
        uint16_t low  = (msg_rx[5] << 8) | msg_rx[6];

        char payload[64];
        snprintf(payload, sizeof(payload), "{\"high\": %u, \"low\": %u}", high, low);

        Serial.print("Publicando no MQTT: ");
        Serial.println(payload);

        publish_data(&MQTT, topic_sensor, payload);
    } else {
        Serial.println("Dados incompletos, não publicando.");
    }
    }  

    MQTT.loop();

    Serial.println();
    delay(2000);    
}