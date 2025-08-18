
#include "utils.h"

const char* ssid = "POCOX6Pro";    
const char* password = "esppucrs";

void WIFIConnect(WiFiClient *espClient)
{
  Serial.println("Conectando a rede wifi!");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Conectando a rede wifi....");
  }
  Serial.println("Conectado a rede wifi");
}

void MQTTConnect(PubSubClient *MQTT)
{
  while(!MQTT->connected()) 
  {
    if (MQTT->connect(ID_MQTT))
    {
      Serial.println("Conectado ao Broker!");
      //MQTT->subscribe(topic_led);
    } 
    else 
    {
      Serial.print("Falha na conexão. O status é: ");
      Serial.print(MQTT->state());      
    }
  }
}

void publish_data(PubSubClient *MQTT,const char *topic, String data)
{
  MQTT->publish(topic, data.c_str());
}

void callback(char *topic, byte *payload, unsigned int length) 
{
  // if(String(topic) == "lab318/led") 
  // {      
  //   .... 
  // } 
  
}

void printHexMessage(unsigned char values[], int tamanho) {
  for (int i = 0; i < tamanho; i++) {
    printHexByte(values[i]);
  }
  Serial.println();
}

void printHexByte(unsigned char b)
{
  Serial.print((b >> 4) & 0xF, HEX);
  Serial.print(b & 0xF, HEX);
  Serial.print(' ');
}

void calculateCRC(unsigned char *frame, int tamanho, int *crc_saida) {
  unsigned int crc = 0xFFFF;            // Initialize CRC to 0xFFFF
  for (int n = 0; n < tamanho; n++) {
    crc ^= frame[n];                    // XOR the frame byte with the CRC
    for (int m = 0; m < 8; m++) {
      if (crc & 0x0001) {               // Check if the LSB of the CRC is 1
        crc >>= 1;                      // Right shift the CRC
        crc ^= 0xA001;                  // XOR the CRC with the polynomial 0xA001
      } else {
        crc >>= 1;                      // Right shift the CRC
      }
    }
  }
  crc_saida[1] = (crc >> 8) & 0xFF;     // Separando a parte alta do CRC  
  crc_saida[0] = crc & 0xFF;
}