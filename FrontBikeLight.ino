#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <avr/io.h>
//#include "TimerOne.h"
#include "bike_light_constants.h"

#include <SoftwareSerial.h>
#include <SoftModem.h>
#include <ctype.h>

#include "Tlc5940.h"
#include "tlc_animations.h"
#include "SimpleTimer.h"

SoftModem modem;
SimpleTimer signalTimer;

#define MYSERIALON
#ifdef MYSERIALON
SoftwareSerial mySerial(SERIAL_RX, SERIAL_TX); // RX, TX
#define mySerial mySerial
#else 
#define mySerial Serial
#endif
//#define radio void
// ============================================================================ //
// RADIO SETUP
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
RF24 radio(RADIO_PIN_CE,RADIO_PIN_CSN);

// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
//const uint64_t pipe = 0xF0F0F0F0AA;

// The debug-friendly names of those roles
int retryCounter = 4;

// ============================================================================ //
// MODE HELPERS

bool isModeSwitchState(unsigned int mode) {
  if ( mode >= LED_STATE_ALL_OFF && mode < LED_TOTAL_STATES) {
    return true;
  }
  return false;
}

void incrementModeState() {
    ledDefaultState++;
    if (!isModeSwitchState(ledDefaultState) ) {
       ledDefaultState = LED_STATE_ALL_OFF;
    }
}

void modeButtonPress() {    
    printf("MODE BUTTON PRESS: ");
    incrementModeState();
    printf("%d\n\r",ledDefaultState);
    currentLedState = ledDefaultState;
}

// ============================================================================ //
// BUTTON PRESS HELPERS

void hornButtonPress() {    
    printf("HORN BUTTON PRESS: ");
    currentHornState = HORN_STATE_SIREN;
}


bool checkButton(int pin) {
  if (digitalRead(pin) == HIGH) {
    mySerial.print("Button Pressed: ");
    mySerial.print(pin);
    mySerial.println(" - HIGH");    
    while (digitalRead(pin) == HIGH) {
      delay(30);
    }
    return true;
  }  
  return false;
}

bool buttonStateChanged() {
  bool rightButton = rightButtonPressed();
  if (rightButton) {
    mySerial.println("Right Press");
    return true;
  }
  bool leftButton = leftButtonPressed();
  if (leftButton) {
    mySerial.println("Left Press");
    return true;
  }
  bool modeButton = modeButtonPressed();
  if (modeButton) {
    deleteCurrentTimer();

    mySerial.println("Mode Press");
    return true;
  }
  bool cancelButton = cancelButtonPressed();
  if (cancelButton) {
    deleteCurrentTimer();

    mySerial.println("Cancel Press");
    return true;
  }
  return false;
}

bool modeButtonPressed() {
  bool check = checkButton(MODE_PIN);
  if (check) {
      modeButtonPress();
      return true;
  }  
  return false;
}

void startSignalTimer(int time) {
  currentTimerID = signalTimer.setTimeout(time, deleteCurrentTimer);
  mySerial.print("Starting timer: ");
  mySerial.println(currentTimerID);
}

bool leftButtonPressed() {
  bool check = checkButton(LEFT_PIN);
  if (check) {
      currentLedState = LED_STATE_LEFT;
      startSignalTimer(8000);
      return true;
  }
  return false;
}

bool rightButtonPressed() {
  bool check = checkButton(RIGHT_PIN);
  if (check) {
      currentLedState = LED_STATE_RIGHT;
      startSignalTimer(8000);
      return true;
  }
  return false;  
}

bool cancelButtonPressed() {
  bool check = checkButton(CANCEL_PIN);
  if (check) {
      currentLedState = ledDefaultState;
      return true;
  }
  return false;
}

int hornClickCounter = 0;
int elapsedTimeSincePress = 0;

// ============================================================================ //
// INIT HELPERS

void radioInit () {
//  mySerial.println("Starting Radio Initialization"); 
//
//  radio.begin();
//  radio.setPALevel(RF24_PA_MAX);
//  // optionally, increase the delay between retries & # of retries
//  radio.setRetries(15,15);
//  // optionally, reduce the payload size.  seems to
//  // improve reliability
//  //  radio.setPayloadSize(8);
//  radio.openWritingPipe(0xF0F0F0AA);
//  radio.printDetails();
//  char *moreDetail = radio.getStats();
//  mySerial.println(moreDetail);
//  mySerial.println("Finished Radio Initialization"); 
}

void buttonPinInit () {
  mySerial.println("Starting Button Pin Initialization"); 

  pinMode(MODE_PIN, INPUT);
  pinMode(LEFT_PIN, INPUT);
  pinMode(RIGHT_PIN, INPUT);
  pinMode(CANCEL_PIN, INPUT);
  
//  digitalWrite(MODE_PIN, LOW);
//  digitalWrite(LEFT_PIN, LOW);
//  digitalWrite(RIGHT_PIN, LOW);
//  digitalWrite(CANCEL_PIN, LOW);
  
//  pinMode(HORN_IN_PIN, INPUT);
//  pinMode(HORN_OUT_PIN, OUTPUT);
  
  //numbers 0 (on digital pin 2) and 1 (on digital pin 3)
//  attachInterrupt(0, leftButtonPress, FALLING);
//  attachInterrupt(1, rightButtonPress, FALLING); 
  mySerial.println("Finished Button Pin Initialization"); 
}

void softwareSerialInit() {
//  mySerial.println("Starting Software Serial Initialization"); 
  mySerial.begin(4800);
  mySerial.println("Finished Software Serial Initialization"); 
}

void audioModemInit() {
    mySerial.println("Starting Audio Modem Initialization"); 

    modem.begin();
    
    mySerial.println("Finished Audio Modem Initialization"); 
}

void tlcInit () {
  mySerial.println("Starting TLC Modem Initialization"); 

  Tlc.init();
  delay(20);
  allOn();
  delay(50);
  allOff();
  delay(50);
  allOn();
  delay(50);
  allOff();
  mySerial.println("Finished TLC Initialization"); 

}

// ============================================================================ //
// OTHER HELPERS

void deleteCurrentTimer(){
  if (currentTimerID!=NO_TIMER)
  {
    mySerial.print("Deleting Current Timer: ");
    mySerial.println(currentTimerID);
    signalTimer.deleteTimer(currentTimerID);
    currentTimerID = NO_TIMER;
    currentLedState = ledDefaultState;
  }
}


// ============================================================================ //
// SETUP

void setup() {
  // set up the role pin
  // DON'T PRINT OVER SERIAL, PINS 0 and 1 will not work. <--blog this
//  Serial.begin(57600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
//  }
//  Serial.println("Bike Buddy Starting Initialization"); 

#ifdef MYSERIALON
  softwareSerialInit();
#endif
  buttonPinInit();
  tlcInit();

  mySerial.println("\nBike Buddy Remote Initialization\n\r");
}



LedState convertCharacterToCommand(char c) {
  switch (c) {
    case 'A': 
      return LED_STATE_RIGHT;
    case 'B': 
      return LED_STATE_LEFT;
    case 'C': 
      return LED_STATE_ALL_OFF;
    case 'D': 
      return LED_STATE_ALL_ON;
    case 'E': 
      return LED_STATE_PIN_WHEEL;
    case 'F': 
      return LED_STATE_BLINK;
    default:
      return LED_STATE_NONE;

      break;
  }
}

LedState checkModemForData()  {
  
   if (modem.available()){
//    Serial.println("CHECKING MODEM"); 

    int c = modem.read();
    if(isprint(c)){
      LedState command = convertCharacterToCommand(c);     
//      Serial.println((char)c);
      return command;
    }
    else{
      
//      Serial.print("(");
//      Serial.print(c,HEX);
//      Serial.println(")");     
    }
  }
  

//  if(Serial.available()){
//    modem.write(0xff);
//    while(Serial.available()){
//      char c = Serial.read();
//      modem.write(c);
//    }
//  } 
  return LED_STATE_NONE;
}

void displayLedsForState(int state) {
      mySerial.print("Current State: ");
      mySerial.println(state);

      switch (state) {
      case LED_STATE_RIGHT: {
         fadeAnimation(RIGHT_ARROW, NUM_LEDS, 9, false);
         break;
      }
      case LED_STATE_LEFT: {
         fadeAnimation(LEFT_ARROW, NUM_LEDS, 9, false);
         break;
      }
      case LED_STATE_ALL_ON: {
         allOn();
         delay(50);
         break;
      }
      case LED_STATE_ALL_OFF: {
         allOff();
         delay(50);
         break;
      }
      case LED_STATE_BLINK: {
         playAnimation(40, BLINK_FADE_ANN, 11, false);
         playAnimation(20, BLINK_FADE_ANN, 11, true); 
         break;
      }
      case LED_STATE_PIN_WHEEL: {
         playAnimation(70, CLOCK_PULSE_ANN, 10, false);
         break;
      }
      default:
        mySerial.println("Wacky State!!!");
        break;
    }

   // end led role 
}

void loop()
{  

  if (currentTimerID!=NO_TIMER) {
    signalTimer.run();
  }

  bool receivedAudioModemInput = false;
  LedState newState = checkModemForData();
  if (LED_STATE_NONE != newState) {
      mySerial.print("Received New State: "); 
      mySerial.println(newState); 
      currentLedState = newState;
      receivedAudioModemInput = true;
  }
  bool buttonStateDidChange = buttonStateChanged();
  if (retryCounter != 3 || buttonStateDidChange || receivedAudioModemInput)
  {
    // broadcastMessage(currentLedState);
  }

  displayLedsForState(currentLedState);

  // delay(STANDARD_DELAY_MILLI);
}

bool broadcastMessage(int message) {
 int messageLen = sizeof(message);
 bool success = true;
 mySerial.print("Sending Message: ");
 mySerial.println(message);

 bool ok = radio.write( &message, messageLen );
 if (ok) {
   mySerial.println("ok\n\r");
   retryCounter = 3;
   radio.powerDown();
 }
 else {
   success = false;

   bool tx_ok =1, tx_fail=1, rx_ready=1;
   radio.whatHappened(tx_ok, tx_fail, rx_ready);
   char buf[100];
   sprintf(buf,"fail retries left=%d, tx_ok=%d, tx_fail=%d, rx_ready=%d\n\r",retryCounter, tx_ok, tx_fail, rx_ready);
   mySerial.println(buf);

   if (retryCounter>0) {
       char buf2[100];
       sprintf(buf2,"retry\n\r",tx_ok, tx_fail, rx_ready);
       mySerial.println(buf2);
       retryCounter--; 
   } else {

     radio.powerDown();
     // todo issue a fail beep regardless that the message failed
     retryCounter = 3;
     mySerial.println("failed\n\r");
   }
 }
  return success;
}

// ============================================================================ //
// TLC Functions

bool canDelayFor(int time) {
  bool buttonStateDidChange = buttonStateChanged();
  if (buttonStateDidChange) {
    return false;
  }
  delay(time);
  return true;
}

void playAnimation(int withDelay, const int **animation, const int annLen, bool invert) {  
  if (invert)  {
    for (int i=annLen-2; i> 0; i--) {
      toggleTLCLeds(animation[i]);
      if (!canDelayFor(withDelay)) {
        return;
      }
    }
  } else {
    for (int i=0; i< annLen; i++) {
      toggleTLCLeds(animation[i]);
      if (!canDelayFor(withDelay)) {
        return;
      }
    }
  }
}

void toggleTLCLeds(const int *ledsToToggle) {
      /* Tlc.clear() sets all the grayscale values to zero, but does not send
       them to the TLCs.  To actually send the data, call Tlc.update() */
   Tlc.clear();
   for (int channel=0; channel< NUM_LEDS; channel++) {
      int mappedChannel = mapChannelIndexToLEDPosition[channel];
      int strength =  ledsToToggle[channel] * 455;
      if (strength > 0) {
        Tlc.set(mappedChannel, strength);
      } 
   }
       /* Tlc.update() sends the data to the TLCs.  This is when the LEDs will
       actually change. */
    while (Tlc.update());
}

void fadeAnimation(const int *ledArray, int annlen, int stepFromTo, bool fadeIn) {
  int *arrCopy = (int*)calloc(sizeof(int), annlen);
  if (!fadeIn) {
      memcpy(arrCopy, ledArray, sizeof(int) *annlen);
  }
  for (int j=0; j<stepFromTo; j++) {
    for (int i=0; i< annlen; i++) {
      if (fadeIn) {
        if (arrCopy[i]<stepFromTo && ledArray[i]>0) {    
         arrCopy[i]++;
        }
      } 
      else {
        if (arrCopy[i]>0) {    
         arrCopy[i]--;
        }
      }
    }
    toggleTLCLeds(arrCopy);
    if (!canDelayFor(30)) {
      free (arrCopy);
      return;
    }
  }
  free (arrCopy);
}

void allOff() {
    Tlc.clear();
    Tlc.update();
}
void allOn() {
    Tlc.setAll(4095);
    Tlc.update();
}
