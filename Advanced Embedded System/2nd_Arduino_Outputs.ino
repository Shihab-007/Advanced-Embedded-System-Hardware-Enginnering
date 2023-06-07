#include <WiFiNINA.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Neaz";
const char* password = "Neaz145126";

// MQTT broker configuration
const char* mqttServer = "192.168.109.100";
const int mqttPort = 1883;
const char* mqttClientId = "ArduinoClient2";
const char* mqttTopicControl = "control";
const char* mqttTopicLedStatus = "status/led";
const char* mqttTopicBuzzerStatus = "status/buzzer";

const int ledPin = 2;     // Pin connected to LED
const int buzzerPin = 3;  // Pin connected to piezo buzzer

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for Serial Monitor to open
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

  // Set MQTT callback function
  mqttClient.setCallback(callback);

  // Connect to MQTT broker
  if (mqttClient.connect(mqttClientId)) {
    Serial.println("Connected to MQTT broker");
    // Subscribe to control topic
    mqttClient.subscribe(mqttTopicControl);
  } else {
    Serial.println("Failed to connect to MQTT broker");
  }

  // Set pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initialize pin states
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // Publish initial status
  publishLedStatus();
  publishBuzzerStatus();
}

void loop() {
  mqttClient.loop();
}

// Callback function for MQTT message reception
void callback(char* topic, byte* payload, unsigned int length) {
  // Check if the received message is for the control topic
  if (strcmp(topic, mqttTopicControl) == 0) {
    // Convert payload to string
    String command = "";
    for (int i = 0; i < length; i++) {
      command += (char)payload[i];
    }

    // Process the received command
    if (command == "LED_ON") {
      digitalWrite(ledPin, HIGH); // Turn on LED
      Serial.println("LED turned ON");
      publishLedStatus();
    } else if (command == "LED_OFF") {
      digitalWrite(ledPin, LOW); // Turn off LED
      Serial.println("LED turned OFF");
      publishLedStatus();
    } else if (command == "BUZZER_ON") {
      digitalWrite(buzzerPin, HIGH); // Turn on buzzer
      Serial.println("Buzzer turned ON");
      publishBuzzerStatus();
    } else if (command == "BUZZER_OFF") {
      digitalWrite(buzzerPin, LOW); // Turn off buzzer
      Serial.println("Buzzer turned OFF");
      publishBuzzerStatus();
    }
  }
}

// Publish LED status to MQTT topic
void publishLedStatus() {
  if (digitalRead(ledPin) == HIGH) {
    mqttClient.publish(mqttTopicLedStatus, "ON");
  } else {
    mqttClient.publish(mqttTopicLedStatus, "OFF");
  }
}

// Publish buzzer status to MQTT topic
void publishBuzzerStatus() {
  if (digitalRead(buzzerPin) == HIGH) {
    mqttClient.publish(mqttTopicBuzzerStatus, "ON");
  } else {
    mqttClient.publish(mqttTopicBuzzerStatus, "OFF");
  }
}
