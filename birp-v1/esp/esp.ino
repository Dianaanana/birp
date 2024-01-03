#include <WiFi.h>
#include <PubSubClient.h>

#define RXp2 16
#define TXp2 17

// WiFi
const char *ssid = "The Umbrella Academy"; // Enter your WiFi name
const char *password = "frogge404";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "birptopic";
const char *mqtt_username = "birp";
const char *mqtt_password = "birp";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  Serial2.setTimeout(10);
  
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  
  // publish and subscribe
  client.publish(topic, "Hi EMQX I'm esp32BIRP v4 ^^");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  Serial.println("Message Received: ");
  Serial2.println("mesajul trimis ");
  String msg = Serial2.readString();
  char msg_char[16];
  msg.trim();
  msg.toCharArray(msg_char, msg.length());
  Serial.println("string: ");
  Serial.println(msg);
  Serial.println("char: ");
  Serial.println(msg_char);
  client.publish(topic, msg_char);
  client.loop();
  delay(500);
}
