#include <SPI.h>

void setup() {
  pinMode(10, OUTPUT); // set the SS pin as an output
  SPI.begin();         // initialize the SPI library
  Serial.begin(115200);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  digitalWrite(10, HIGH);
}

float val = 5;
float retVal;

void loop() {

  digitalWrite(10, LOW);            // set the SS pin to LOW
  retVal = SPI.transfer(val);
  digitalWrite(10, HIGH);           // set the SS pin HIGH
}
