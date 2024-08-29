#ifndef Mqtt_Module_h
#define Mqtt_Module_h

#include <Arduino.h>
#include <functional>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define MAX_CALLBACKS 10

class MqttModule {
  private:
    PubSubClient* pMqttClient ;
    int callbackCount =0;
    int subCount =0;
    typedef std::function<void(char*, uint8_t*, unsigned int, PubSubClient*)> CallbackType;
    typedef std::function<void(PubSubClient*)> SubscribeType;
    CallbackType callbacks[MAX_CALLBACKS];
    SubscribeType subscribers[MAX_CALLBACKS];
    const char* mqtt_broker;
    int   mqtt_port;
    const char* mqtt_username;
    const char* mqtt_password;

  public:
    MqttModule(WiFiClient* espClient, 
    const char* mqtt_broker, int mqtt_port, 
    const char* mqtt_username, const char* mqtt_password) {
      this->mqtt_broker = mqtt_broker;
      this->mqtt_port = mqtt_port;
      this->mqtt_username = mqtt_username;
      this->mqtt_password = mqtt_password;
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
      } 
      pMqttClient->loop(); 
    }

    void registeCallback(CallbackType callback) {
      if (callbackCount < MAX_CALLBACKS) {
        callbacks[callbackCount++] = callback;
      } else {
        Serial.println("Callbacks limit reached!");
      }
    }

    void registeSubscribers(SubscribeType sub) {
      if (subCount < MAX_CALLBACKS) {
        subscribers[subCount++] = sub;
      } else {
        Serial.println("Subscribes limit reached!");
      }
    }

    PubSubClient* getClient() {
      return pMqttClient;
    }

    private:
    void executeSubscribers() {
      for (int i=0; i<subCount; i++) {
        subscribers[i](pMqttClient);
      }
    }


    void reconnect() {
      // Loop until we're reconnected
      while (!pMqttClient->connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (pMqttClient->connect("ESP8266Client", mqtt_username, mqtt_password)) {
          Serial.println("connected");
          // Subscribe
          executeSubscribers();
        } else {
          Serial.print("failed, rc=");
          Serial.print(pMqttClient->state());
          Serial.println(" try again in 5 seconds");
          // Wait 5 seconds before retrying
          delay(5000);
        }
      }
    }

    void callback(char* topic, byte* message, unsigned int length) {
      for (int i=0; i<callbackCount; i++) {
        callbacks[i](topic, message, length, pMqttClient);
      }
    }
};

#endif