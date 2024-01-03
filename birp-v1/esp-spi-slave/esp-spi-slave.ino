#include <SPI.h>

volatile boolean received;
volatile byte Slavereceived, Slavesend;

void IRAM_ATTR onReceive(int numBytes) {
  Slavereceived = SPI.transfer(0x00); // Value received from master if store in variable slavereceived
  received = true; //Sets received as True 
}

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Start SPI
  SPI.onData(onReceive); // On receive, run function
  received = false;
}

void loop() {
    SPI.transfer(Slavesend); //Sends the x value to master via SPI.transfer
    delay(1000);
  }
}
