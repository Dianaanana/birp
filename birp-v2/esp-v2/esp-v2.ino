#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <ESP32Servo.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
#define RX_PIN 16
#define TX_PIN 17
#define BUTTON_PIN 15


DHT dht(DHTPIN, DHTTYPE);
Servo servo;

int button_state = 0;

// WiFi
const char *ssid = "InTineSunt2Lupi"; // Enter your Wi-Fi name
const char *password = "Inpulasaipupi";   // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "birptopic";
const char *mqtt_username = "birp";
const char *mqtt_password = "birp";
const int mqtt_port = 1883;

//date globale pentru operabilitate
int vector_numere[3];
int mynumber;
String payloadAsString = "";
DynamicJsonDocument doc(1024);
char json[256];
bool refresh_scriere = 0;
bool refresh_citire = 0;
String Mesaj_primit = "";
int nrcitire = 0;

// conectare wifi
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(BUTTON_PIN, INPUT);

  // Serial is used for debug
  Serial.begin(115200);
  // Serial2 is used for communication between esp32 and arduino
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
       
  servo.attach(4);
  dht.begin();

  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the Wi-Fi network");
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "mqttx_e9fc5372";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public EMQX MQTT broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // Publish and subscribe
  client.publish(topic, "Hi, I'm Diana ^^");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Mesaj_primit = "";
  refresh_citire = 1;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    Mesaj_primit = Mesaj_primit + (char)payload[i];
  }

  Serial.println("MESAJUL TAU ESTE :");
  Serial.println(Mesaj_primit);
}

void citire()
{
  deserializeJson(doc, Mesaj_primit);

  float temp = doc["temp"];
  float hum = doc["hum"];
  float air = doc["air"];
  Serial.println(temp);
  Serial.println(hum);
  Serial.println(air);

  refresh_citire = 0;
}



void loop() {
  // TODO servo
  //  servo.write(180);
  //  delay(2000);
  //  servo.write(0);
  //  delay(2000);

  
  // button for demo
  button_state = digitalRead(BUTTON_PIN);
  if (button_state == HIGH) {
    servo.write(180);
    delay(2000);
    servo.write(0);
  }

  Serial.println("Message Received: ");
  String air = Serial2.readString();
  Serial.println(air);

  float umiditate = dht.readHumidity();
  float temperatura = dht.readTemperature();

  Serial.print("umiditate: ");
  Serial.print(umiditate);
  Serial.print("\n");


  Serial.print("temperatura: ");
  Serial.print(temperatura);
  Serial.print("\n");

//  if (refresh_citire == 1) {
//    citire();
//  }

  // conditie de timp timer
//  if (//todo conditie) {
    doc["temp"] = temperatura;
    doc["hum"] = umiditate;
    doc["air"] = air; // de pe seriala 2?

    serializeJson(doc, json);
    client.publish(topic, json);
//}

client.loop();
// connecting to a mqtt broker
client.setServer(mqtt_broker, mqtt_port);
client.setCallback(callback);

delay(100);

}
