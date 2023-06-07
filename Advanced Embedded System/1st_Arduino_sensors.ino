#include <WiFiNINA.h>
#include <Wire.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "BKA_Ueberwachungswagen_07";
const char* password = "Fliesentischbesitzer_1337";

// MQTT broker configuration
const char* mqttServer = "192.168.2.93";
const int mqttPort = 1883;
const char* mqttTopicGas = "sensor/gas";
const char* mqttTopicFlame = "sensor/flame";
const char* mqttClientId = "ArduinoClient";

// MQ-4 Gas Sensor
const int gasSensorPin = A0;

// KY-026 Flame Sensor
const int flameSensorPin = A1;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // Wait for Serial to initialize
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Set MQTT server and port
  mqttClient.setServer(mqttServer, mqttPort);

  // Initialize I2C communication
  Wire.begin();
}

void loop() {
  // Connect to MQTT broker
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Read gas sensor value
  int gasSensorValue = analogRead(gasSensorPin);
  int gasPercentage = map(gasSensorValue, 0, 1023, 0, 100);

  // Read flame sensor value
  int flameSensorValue = analogRead(flameSensorPin);
  int flamePercentage = map(flameSensorValue, 1023, 0, 0, 100);

  // Publish sensor readings to MQTT topics
  char payload[4];
  snprintf(payload, sizeof(payload), "%d", gasPercentage);
  mqttClient.publish(mqttTopicGas, payload);

  snprintf(payload, sizeof(payload), "%d", flamePercentage);
  mqttClient.publish(mqttTopicFlame, payload);

  // Print sensor values to Serial Monitor
  Serial.print("Gas Sensor: ");
  Serial.print(gasPercentage);
  Serial.println("%");

  Serial.print("Flame Sensor: ");
  Serial.print(flamePercentage);
  Serial.println("%");

  delay(1000);  // Delay between readings
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect(mqttClientId)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}
