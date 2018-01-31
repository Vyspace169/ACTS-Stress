#ifndef WIFI_CONTROLLER_HPP
#define WIFI_CONTROLLER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "SystemVariables.hpp"

#include "aws_iot_config.h"
#include "aws_iot_error.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

/// @brief MQTTController that handles the writing to the sd.
class MQTTController{
public:
  MQTTController();
  void publish(double value);
  void connectMQTT();
  void disconnect();
  //void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData);
  ~MQTTController();
private:
    AWS_IoT_Client client;
    IoT_Publish_Message_Params paramsQOS0;
    IoT_Publish_Message_Params paramsQOS1;

    const char* TAG = "MQTT Controller";
    const char* client_id = "1";
    const char* TOPIC = "client/result";
    const int TOPIC_LEN = strlen(TOPIC);
    const char* user = "Sensor";
    short unsigned int port = 8883;
    char* host = "a3oyj9w6dlgtya.iot.eu-central-1.amazonaws.com";
};
#endif //WIFI_CONTROLLER_HPP
