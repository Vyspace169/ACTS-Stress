#include <WiFi.h>
#include <PubSubClient.h>

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

}

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
}
