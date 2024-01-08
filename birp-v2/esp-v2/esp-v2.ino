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
#define BUZZER_PIN 19
#define TRESHOLD 400

int val = 10;
int length = 9;

char notes[] = "cega cega";
int beats[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int tempo = 20;
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(tone);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(tone);
  }
}
void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1115, 900, 719, 632, 475, 336, 214, 156};

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}


DHT dht(DHTPIN, DHTTYPE);
Servo servo;

int button_state = 0;

// WiFi
const char *ssid = "Birpdemo"; // Enter your Wi-Fi name
const char *password = "Birpdemo";   // Enter Wi-Fi password

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

// timer interrupt
volatile int interruptCounter;  //for counting interrupt
int totalInterruptCounter;    //total interrupt counting
int LED_STATE = LOW;
hw_timer_t * timer = NULL;      //H/W timer defining (Pointer to the Structure)

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {      //Defining Inerrupt function with IRAM_ATTR for faster access
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

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
  client.publish(topic, "Hi, I'm Birp ^^");
  client.subscribe(topic);

  // timer intterupt
  timer = timerBegin(0, 80, true);             // timer 0, prescalar: 80, UP counting
  timerAttachInterrupt(timer, &onTimer, true);   // Attach interrupt
  timerAlarmWrite(timer, 60000000, true);     // Match value= 1000000 for 1 sec. delay.
  timerAlarmEnable(timer);                 // Enable Timer with interrupt (Alarm Enable)
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
//  Serial.println(temp);
//  Serial.println(hum);
//  Serial.println(air);

  refresh_citire = 0;
}



void loop() {

  // button for demo
  button_state = digitalRead(BUTTON_PIN);
//  Serial.print("button_state: ");
//  Serial.println(button_state);
  // reverse button
  if (button_state == LOW) {
    servo.write(180);
    delay(2000);
    servo.write(0);

    // buzzer logic
    for (int i = 0; i < length; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo); // rest
      } else {
        playNote(notes[i], beats[i] * tempo);
      }

      // pause between notes
      delay(tempo / 2);
    }
  }

//  Serial.println("Message Received: ");
  String air_str = Serial2.readString();
  int air = air_str.toInt();
//  Serial.println(air_str);
//     Serial.println(air);

  if(air > 400) {
    servo.write(180);
  }
  if(air < 400) {
    servo.write(0);
  }

  float umiditate = dht.readHumidity();
  float temperatura = dht.readTemperature();

//  Serial.print("umiditate: ");
//  Serial.print(umiditate);
//  Serial.print("\n");
//
//
//  Serial.print("temperatura: ");
//  Serial.print(temperatura);
//  Serial.print("\n");

  if (interruptCounter > 0) {

    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    //    totalInterruptCounter++;           //counting total interrupt

    doc["temp"] = temperatura;
    doc["hum"] = umiditate;
    doc["air"] = air; // de pe seriala 2?

    serializeJson(doc, json);
    client.publish(topic, json);

    //    Serial.print("An interrupt as occurred. Total number: ");
    //    Serial.println(totalInterruptCounter);
  }

  client.loop();
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  delay(100);

}
