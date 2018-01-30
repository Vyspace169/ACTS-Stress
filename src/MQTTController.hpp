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

#include "aws_iot_config.h"
#include "aws_iot_error.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

/// @brief MQTTController that handles the writing to the sd.
class MQTTController{
public:
  MQTTController(int user);
  void publish(long time, int value);
  void connectMQTT();
  void disconnect();
  //void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData);
  ~MQTTController();
private:
    //static EventGroupHandle_t wifi_event_group;
    const char* TAG = "MQTT Controller";
    const char* client_id = "1";
    const char* topic = "ref_v";
    const char* user = "Sensor";
    short unsigned int port = 8883;
    char* host = "a3oyj9w6dlgtya.iot.eu-west-2.amazonaws.com:8883";

    char* ssid = "DasInternet";
    char* pass = "Hvbek83#etJK";
};
#endif //WIFI_CONTROLLER_HPP
