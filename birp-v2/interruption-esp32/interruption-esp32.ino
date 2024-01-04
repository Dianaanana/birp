#define LED_PIN 2               // In some ESP32 board have inbuilt LED

volatile int interruptCounter;  //for counting interrupt
int totalInterruptCounter;    //total interrupt counting
int LED_STATE=LOW;
hw_timer_t * timer = NULL;      //H/W timer defining (Pointer to the Structure)

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {      //Defining Inerrupt function with IRAM_ATTR for faster access
 portENTER_CRITICAL_ISR(&timerMux);
 interruptCounter++;
 portEXIT_CRITICAL_ISR(&timerMux);
}
 
void setup() {
 Serial.begin(115200);
 pinMode (LED_PIN, OUTPUT);
 
 timer = timerBegin(0, 80, true);             // timer 0, prescalar: 80, UP counting
 timerAttachInterrupt(timer, &onTimer, true);   // Attach interrupt
 timerAlarmWrite(timer, 60000000, true);     // Match value= 1000000 for 1 sec. delay.
 timerAlarmEnable(timer);                 // Enable Timer with interrupt (Alarm Enable)
}
 
void loop() {
 if (interruptCounter > 0) {
 
   portENTER_CRITICAL(&timerMux);
   interruptCounter--;
   portEXIT_CRITICAL(&timerMux);
 
   totalInterruptCounter++;           //counting total interrupt

   LED_STATE= !LED_STATE;         //toggle logic
   digitalWrite(LED_PIN, LED_STATE);    //Toggle LED
   Serial.print("An interrupt as occurred. Total number: ");
   Serial.println(totalInterruptCounter);
 }
}
