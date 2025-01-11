#ifndef base_device_h
#define base_device_h

#include "mqtt_module.h"

/*
Features:
1. pass parameters like channel and deviceName to the constructor
2. take a call to begin() - it will register handlers in HA
3. implement the onState() method - it will be called every time HA publishes state
4. if the device can change its internal state - call setState() to publish a new state
5. if HA has a configuration, then after receiving it onConfig() will be called
*/

class BaseDevice {

  private:
    MqttModule* pMqtt;
    const char* deviceName;
    const char* topicTemplate;
    bool commandEnabled = false;
    bool configEnabled = false;
    String channel;

  public: 
  BaseDevice(MqttModule* pMqtt, const char* channel, const char* deviceName, const char* topicTemplate, 
             bool commandEnabled,  bool configEnabled) {
    this->deviceName = deviceName;
    this->topicTemplate = topicTemplate;
    this->channel = String(channel);
    this->pMqtt = pMqtt;
    this->commandEnabled = commandEnabled;
    this->configEnabled = configEnabled;
  }

  void begin() {
    pMqtt->registeSubscribers([this](PubSubClient* subProvider) {
        if (commandEnabled == true) {
          registerSubscription("command", subProvider);
        }
        if (configEnabled == true) {
          registerSubscription("config", subProvider);
        }
      }
    );

    pMqtt->registeCallback([this](char* topic, uint8_t* message, unsigned int length) {
      
      if (commandEnabled) {
        String m = handleMessage(topic, message, length, "command");
        if (m != "") {
          Serial.print("Received command: ");
          Serial.print(topic);
          Serial.print(" : ");
          Serial.println(m);
          if (onState(m)) {
            setState(m);
          }  
        }
      }

      if (configEnabled) {
        String m = handleMessage(topic, message, length, "config");
        if (m != "") {
          Serial.print("Received configuration: ");
          Serial.print(topic);
          Serial.print(" : ");
          Serial.println(m);
          onConfig(m);
        }
      }
    });
  }

  virtual bool onState(String state)=0;
  virtual void onConfig(String state)=0;

  void configRequest() {
    char buff[50];
    sprintf(buff, topicTemplate, "getConfig", deviceName, channel.c_str());
    Serial.print("Config request: ");
    Serial.println(buff);
    pMqtt->getClient()->publish(buff, "", true);  
  }

  protected:
  void setState(String state/*, MqttModule* pMqtt*/) {
    char buff[50];
    sprintf(buff, topicTemplate, "state", deviceName, channel.c_str());

    Serial.print("Sending state: ");
    Serial.print(state);
    Serial.print(" to ");
    Serial.println(buff);
    pMqtt->getClient()->publish(buff, state.c_str(), true);  
    
  }

  private:

  void registerSubscription(const char* type, PubSubClient* subProvider) {
    char buff[50];
    sprintf(buff, topicTemplate, type, deviceName, channel.c_str());
    subProvider->subscribe(buff);
    Serial.print("Subscribe ");
    Serial.print(type);
    Serial.print(": ");
    Serial.println(buff);
  }

  String handleMessage(char* topic, uint8_t* message, unsigned int length, const char* type) {

    char receivedChannel[20];
    receivedChannel[0] = 0;
    
    char buff[50];
    
    sprintf(buff, topicTemplate, type, deviceName, "");
    getCommandChannelTopic(receivedChannel, buff, topic);

    if (strcmp (receivedChannel, channel.c_str()) != 0) {
      return "";
    }
  
    Serial.print("Topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");

    String messageTemp;
    for (int i=0; i<length; i++) {
      messageTemp += (char) message[i];
    }
    Serial.println(messageTemp);
    return messageTemp;
  }

  void getCommandChannelTopic(char* receivedChannel, char* startWith, char* topic) {
      
    int startWithLen = strlen(startWith);

    if(strncmp(topic, startWith, startWithLen) != 0) {
      return ;
    }
    int index = 0;
    
    for (int i=startWithLen; i<startWithLen + 10 || topic[i] == 0 ; i++) {
      receivedChannel[index++] = topic[i];
    }

    //EOS
    receivedChannel[index] = 0;
  }

};

#endif