#ifndef Mqtt_Module_h
#define Mqtt_Module_h

#include <Arduino.h>
#include <functional>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "logger.h"

#define MAX_CALLBACKS 50

class MqttModule {
  private:
    PubSubClient* pMqttClient ;
    int callbackCount =0;
    int subCount =0;
    typedef std::function<void(char*, uint8_t*, unsigned int)> CallbackType;
    typedef std::function<void(PubSubClient*)> SubscribeType;
    CallbackType callbacks[MAX_CALLBACKS];
    SubscribeType subscribers[MAX_CALLBACKS];
    const char* mqtt_broker;
    int   mqtt_port;
    const char* mqtt_username;
    const char* mqtt_password;
    const char* mqtt_clientName;
    Logger* logger;

  public:
    MqttModule(WiFiClient* espClient, 
    const char* mqtt_broker, int mqtt_port, 
    const char* mqtt_username, const char* mqtt_password,
    const char* mqtt_clientName, Logger* logger) {
      this->mqtt_broker = mqtt_broker;
      this->mqtt_port = mqtt_port;
      this->mqtt_username = mqtt_username;
      this->mqtt_password = mqtt_password;
      this->mqtt_clientName = mqtt_clientName;
      this->logger = logger;
      pMqttClient = new PubSubClient(*espClient);
    }

    void begin() {
      pMqttClient->setServer(mqtt_broker, mqtt_port);
      pMqttClient->setCallback([this](char* topic, uint8_t* message, unsigned int length){
        this->callback(topic, message,  length);
      });
    }

    void loop() {
      if (!pMqttClient->connected()) {
        reconnect();
        logger->fast();
      } 
      pMqttClient->loop(); 
    }

    void registeCallback(CallbackType callback) {
      if (callbackCount < MAX_CALLBACKS) {
        callbacks[callbackCount++] = callback;
      } else {
        Serial.println("Callbacks limit reached!");
        logger->error();
      }
    }

    void registeSubscribers(SubscribeType sub) {
      if (subCount < MAX_CALLBACKS) {
        subscribers[subCount++] = sub;
      } else {
        Serial.println("Subscribes limit reached!");
        logger->error();
      }
    }

    PubSubClient* getClient() {
      reconnect();
      logger->setBlinkOnce();
      return pMqttClient;
    }

    private:

    void reconnect() {
      // Loop until we're reconnected
      while (!pMqttClient->connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        String clientId = "ESP8266Client-";
        clientId += String(mqtt_clientName);
        clientId += String(random(0xffff), HEX);
        if (pMqttClient->connect(clientId.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("connected");
        } else {
          pMqttClient->state();
          // Wait 5 seconds before retrying
          delay(5000);
        }
        if (pMqttClient->connected()) {
          // Subscribe
          executeSubscribers();
        }
      }
    }

    void executeSubscribers() {
      for (int i=0; i<subCount; i++) {
        subscribers[i](pMqttClient);
      }
    }

    void callback(char* topic, byte* message, unsigned int length) {
      char topicCopy[strlen(topic) + 1];  
      strcpy(topicCopy, topic);
      byte messageCopy[length];
      memcpy(messageCopy, message, length);

      for (int i=0; i<callbackCount; i++) {
        callbacks[i](topicCopy, messageCopy, length);
      }
    }
};

#endif