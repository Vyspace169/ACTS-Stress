/**
* @file MQTTController.hpp
* @data 21 september, 2017
*
* \class MQTTController
*
* \brief Class that handles all MQTT communication
*
* This class uses ESP-IDF built in functions in order
* to communicate with the AWS sever. All server
* declarations are done in the private section of
* this class and the user key code is defined in
* SystemVariables.
*
*/

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

class MQTTController{
public:
 /*!
  * \brief MQTTController constructor
  *
  * Empty, not implemented.
  */
  MQTTController();

  /*!
   * \brief publish method
   * \param value MovementIndex to be sent to the server
   *
   * When connected, this method will send the data to the
   * server. This data will be published to the TOPIC
   * part of the database.
   */
  void publish(double value);

  /*!
   * \brief connectMQTT method
   *
   * This method connects to the MQTT server using
   * the built in functions from ESP-IDF.
   */
  void connectMQTT();

  /*!
   * \brief disconnect method
   *
   * This method disconnects the sensor client from
   * the server.
   */
  void disconnect();

  //void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData);

  /*!
   * \brief MQTTController deconstructor
   *
   * If there is a connection, this deconstructor
   * will disconnect.
   */
  ~MQTTController();
private:
    AWS_IoT_Client client;
    IoT_Publish_Message_Params paramsQOS0;
    IoT_Publish_Message_Params paramsQOS1;

    const char* TAG = "MQTT Controller";
    const char* client_id = "1";
    const char* TOPIC = "acts/result";
    const int TOPIC_LEN = strlen(TOPIC);
    const char* user = "Sensor";
    short unsigned int port = 8883;
    char* host = "a3oyj9w6dlgtya.iot.eu-central-1.amazonaws.com";
};
#endif //WIFI_CONTROLLER_HPP
