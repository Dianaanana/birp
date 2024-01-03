#include <Servo.h>

#define MQ_SENSOR_PIN A0
#define SERVO_PIN 8
#define BUZZER_PIN 7

Servo myservo;
bool servoWriteFlag = false;
const int thresholdValue = 95;

int length = 9;
char notes[] = "cega cega";
//int beats[] = { 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2};
int beats[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

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
//  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  int tones[] = { 1115, 900, 719, 632, 475, 336, 214,156};

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(MQ_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  myservo.attach(SERVO_PIN);
}

void loop() {
  // read the sensor value
  float analogReading = analogRead(MQ_SENSOR_PIN);

  // debug
  Serial.println(analogReading);

  // servo logic
  if (!servoWriteFlag && analogReading > thresholdValue) {
    myservo.write(90);
    servoWriteFlag = true;
  }
  delay(500);
  if (servoWriteFlag && analogReading < thresholdValue) {
    myservo.write(0);
    servoWriteFlag = false;
  }

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

  delay(1000);
}
