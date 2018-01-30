#include <WiFi.h>
#include <PubSubClient.h>

<<<<<<< HEAD
const char* ssid = "dd-wrt";
const char* password = "0000000000";
const char* mqtt_server = "a3oyj9w6dlgtya.iot.eu-west-2.amazonaws.com:8883";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[20];

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  if ((char)payload[0] == '1') {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
=======
WifiController::WifiController(unsigned int task_priority, DataProcessor &dp) :
	BaseTask(task_priority),
	DPHandle{dp}
	{
		main_task();
	}
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202

void run_wifi_task(void *args)  {
	WifiController *sTask = static_cast<WifiController*>(args);
	MovementStack MovementSaver;

    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (WifiActivateFlag | WifiReadyFlag | StandbyWifiTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);

        if((uxBits & WifiActivateFlag)) {
			ESP_LOGI("WIFI TASK", "Connecting to wifi");
			bool enabled = WiFiConnect(WIFI_CONNECT_TIMEOUT);

			// push activity data in its fifo
			MovementSaver.PushData(sTask->DPHandle.GetActivityData());
			sTask->DPHandle.ResetActivityData();

			if(enabled == true) {
				ESP_LOGI("WIFI TASK", "Wifi connected");
				if(TCPConnectToServer(WIFI_TCP_SERVER, WIFI_TCP_PORT) == true) {
					ESP_LOGI("WIFI TASK", "Socket connected");
					char send_string[50];
					while(MovementSaver.DataCount() != 0) {
						sprintf(send_string, "%.2f\n", MovementSaver.GetActivityData());
						ESP_LOGI("WIFI TASK", "Sending data to server: %d: %s", MovementSaver.DataCount(), send_string);
						TCPSend(send_string, strlen(send_string));
						MovementSaver.PopData();
					}
					TCPDisconnect();
				}
				else {
					ESP_LOGI("WIFI TASK", "Socket connection failed");
				}
				WiFiDisconnect();
			}
			else {
				ESP_LOGI("WIFI TASK", "Wifi connection failed");
			}
        }

        if(uxBits & WifiReadyFlag) {
        	// TODO: WifiReadyFlag must be set by something
            // TODO: Send data over wifi to the server
        	// TODO: MQTT frame builder class
        }

        if(uxBits & StandbyWifiTaskUnhandled) {
        	ESP_LOGI("WIFI TASK", "Ready to sleep");
        	xEventGroupClearBits(GlobalEventGroupHandle, StandbyWifiTaskUnhandled);
        	while(1) {
        		vTaskDelay(1000 / portTICK_PERIOD_MS);
        	}
        }
    }
}

<<<<<<< HEAD
void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void init() {
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  pinMode(led, OUTPUT);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(receivedCallback);
}
void run() {
  if (!client.connected()) {
    mqttconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    if (!isnan(temperature)) {
      snprintf (msg, 20, "%lf", temperature);
      client.publish(TEMP_TOPIC, msg);
    }
  }
=======
void wifi_timers_callback( TimerHandle_t xTimer )  {
	xEventGroupSetBits(GlobalEventGroupHandle, WifiActivateFlag);
}

void WifiController::main_task() {
	TimerHandle_t wifi_poll_timer = NULL;
	wifi_poll_timer = xTimerCreate("wifi_poll_clock",
			WIFI_POLL_FREQUENCY_SEC * 1000,
			pdTRUE,
			WIFITASK_TIMER_ID,
			wifi_timers_callback);
	xTimerStart( wifi_poll_timer, 0 );

	if(wifi_poll_timer == NULL) {
		// Something has failed creating the timer
		ESP_LOGI("WIFI TASK", "Timer creation failed");
	} else {
		ESP_LOGI("WIFI TASK", "Timer created");
	}

	TaskHandle_t xHandle = NULL;
	void* thisTask = static_cast<void*>(this);
	BaseType_t xReturned = xTaskCreatePinnedToCore(run_wifi_task,
			"wifi_task",
			WIFITASK_STACK_SIZE,
			thisTask,
			task_priority,
			&xHandle,
			WIFITASK_CORE_NUM);

	if(xHandle == NULL) {
		// Handle assignment has failed
		ESP_LOGI("WIFI TASK", "Handle creation failed");
	}

	if(xReturned != pdPASS) {
		// xReturned false (something went wrong!)
		ESP_LOGI("WIFI TASK", "Task creation failed");
	}

	ESP_LOGI("WIFI TASK", "Task is running");
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
}
