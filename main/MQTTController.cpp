#include "MQTTController.hpp"

extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_pem_crt_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_private_pem_key_end");

MQTTController::MQTTController() {
}

void MQTTController::publish(double value) {
    IoT_Error_t rc = FAILURE;
    char cPayload[200];
    int32_t i = 0;

    paramsQOS1.qos = QOS1;
    paramsQOS1.payload = (void *) cPayload;
    paramsQOS1.isRetained = 0;

    sprintf(cPayload, "{\"key\" :\"%s\",\"value\" : \"%lf\"}", MQTT_USER_KEY_CODE, value);
    paramsQOS1.payloadLen = strlen(cPayload);
    rc = aws_iot_mqtt_publish(&client, TOPIC, TOPIC_LEN, &paramsQOS1);
    if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
        ESP_LOGW(TAG, "QOS1 publish ack not received."); // @suppress("Symbol is not resolved")
        rc = SUCCESS;
    }
}

void MQTTController::disconnect(){
    IoT_Error_t rc = aws_iot_mqtt_autoreconnect_set_status(&client, false);
    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", rc); // @suppress("Symbol is not resolved")
    }
    aws_iot_mqtt_disconnect(&client);
}

static void iot_subscribe_callback_handler(
    AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,IoT_Publish_Message_Params *params, void *pData) {
    char * TAG = "TEST";
    ESP_LOGI(TAG, "Subscribe callback"); // @suppress("Symbol is not resolved")
    ESP_LOGI(TAG, "%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)params->payload); // @suppress("Symbol is not resolved")
}

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
    char * TAG = "disconnect";
    ESP_LOGW(TAG, "MQTT Disconnect"); // @suppress("Symbol is not resolved")
    IoT_Error_t rc = FAILURE;

    if(NULL == pClient) {
        return;
    }

    if(aws_iot_is_autoreconnect_enabled(pClient)) {
        ESP_LOGI(TAG, "Auto Reconnect is enabled, Reconnecting attempt will start now"); // @suppress("Symbol is not resolved")
    } else {
        ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect..."); // @suppress("Symbol is not resolved")
        rc = aws_iot_mqtt_attempt_reconnect(pClient);
        if(NETWORK_RECONNECTED == rc) {
            ESP_LOGW(TAG, "Manual Reconnect Successful"); // @suppress("Symbol is not resolved")
        } else {
            ESP_LOGW(TAG, "Manual Reconnect Failed - %d", rc); // @suppress("Symbol is not resolved")
        }
    }
}

void MQTTController::connectMQTT() {
    IoT_Error_t rc = FAILURE;

    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
    ESP_LOGI(TAG, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG); // @suppress("Symbol is not resolved")

    mqttInitParams.enableAutoReconnect = false; // We enable this later below
    mqttInitParams.pHostURL = host;
    mqttInitParams.port = port;
    mqttInitParams.pRootCALocation = (const char *)aws_root_ca_pem_start;
    mqttInitParams.pDeviceCertLocation = (const char *)certificate_pem_crt_start;
    mqttInitParams.pDevicePrivateKeyLocation = (const char *)private_pem_key_start;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;

    rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "aws_iot_mqtt_init returned error : %d ", rc); // @suppress("Symbol is not resolved")
    }

    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = client_id;
    connectParams.clientIDLen = (uint16_t) strlen(client_id);
    connectParams.isWillMsgPresent = false;

    for(int j = 0; j < 10; j++) {
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if(SUCCESS != rc) {
            ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port); // @suppress("Symbol is not resolved")
            vTaskDelay(1000 / portTICK_RATE_MS); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
        }else{
            break;
        }
    }
    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);

    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d", rc); // @suppress("Symbol is not resolved")
    }
}

MQTTController::~MQTTController(){
    disconnect();
}
