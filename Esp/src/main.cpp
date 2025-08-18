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
int TIMEOUT = 500;

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

    // Serial1 será usada com o adaptador MAX485
    Serial1.begin(9600, SERIAL_8N1, 16, 17);

    // Pino que fica alternando entre transmitir ou receber
    pinMode(MAX485_DE_RE, OUTPUT);
    digitalWrite(MAX485_DE_RE, LOW);
    delay(1000);

} 

void loop() 
{
  static long long pooling = 0;
  
  if(!MQTT.connected()) MQTTConnect(&MQTT);
  if(WiFi.status() != WL_CONNECTED) WIFIConnect(&espClient);
  
  if(millis() > pooling+5000)
  {
    pooling = millis();
    publish_data(&MQTT, topic_sensor, "Teste\n");
  }  

  MQTT.loop();

  uint32_t startTime = 0;
  int j = 0;

  // Imprimindo na Serial0 a mensagem que vai ser enviada ao slave.
  Serial.print("TX: ");
  printHexMessage(msg_tx, sizeof(msg_tx) );

  digitalWrite(MAX485_DE_RE, HIGH);
  delay(10);

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

    for(j=1; j<10; j++)
      printHexByte(msg_rx[j]);

    Serial.println();
    delay(2000);    
}