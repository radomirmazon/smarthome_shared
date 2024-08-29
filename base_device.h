#ifndef base_device_h
#define base_device_h

#include "mqtt_module.h"

class BaseDevice {

  private:
    MqttModule* pMqtt;
    const char* deviceName;
    const char* topicTemplate;
    const char* channel;

  public: 
  BaseDevice(MqttModule* pMqtt, const char* channel, const char* deviceName, const char* topicTemplate) {
    this->deviceName = deviceName;
    this->topicTemplate = topicTemplate;
    this->channel = channel;
    this->pMqtt = pMqtt;
  }

  void begin() {
    pMqtt->registeSubscribers([this](PubSubClient* subProvider) {
        char buff[50];
        sprintf(buff, topicTemplate, "command", deviceName, channel);
        subProvider->subscribe(buff);
        Serial.print("Registe: ");
        Serial.print(deviceName);
        Serial.print(" topic: ");
        Serial.println(buff);
      }
    );

    pMqtt->registeCallback([this](char* topic, uint8_t* message, unsigned int length, PubSubClient* pMqttClient) {
      
        char receivedChannel[20];
        receivedChannel[0] = 0;
        
        char buff[50];
        sprintf(buff, topicTemplate, "command", deviceName, "");
        getCommandChannelTopic(receivedChannel, buff, topic);
        
        /*
        Serial.print(receivedChannel);
        Serial.print(" <=> ");
        Serial.print(channel);
        Serial.print(" <=> ");
        Serial.print(buff);
        Serial.print(" <=> ");
        Serial.println(topic);
        */

        if (strcmp (receivedChannel, channel) != 0) {
          return;
        }
        Serial.print("Topic: ");
        Serial.print(topic);
        Serial.print(". Message: ");

        String messageTemp;
        for (int i=0; i<length; i++) {
          messageTemp += (char) message[i];
        }
        Serial.println(messageTemp);
        
        if (!setState(messageTemp)) {
          return;
        }

        sprintf(buff, topicTemplate, "state", deviceName, channel);

        Serial.print("State: ");
        Serial.print(messageTemp);
        Serial.print(" to ");
        Serial.println(buff);

        pMqttClient->publish(buff, messageTemp.c_str(), true);      
      });
  }

  virtual bool setState(String state)=0;

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