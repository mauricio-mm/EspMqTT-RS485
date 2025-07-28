#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include "FS.h"
#include "SPIFFS.h"

#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>
#include "main.h"

#define LEDPIN 2

#define DHTPIN 4
#define DHTTYPE DHT22

#define SERVOPIN 16

#define ID_MQTT "esp_iot"
#define topic_led "lab318/led"
#define topic_dht "lab318/dht"
#define topic_servo "lab318/servo"

void WIFIConnect(WiFiClient *espClient);
void MQTTConnect(PubSubClient *MQTT);
void publish_data(PubSubClient *MQTT, const char *topic, String data);
void callback(char *topic, byte *payload, unsigned int length);

#endif