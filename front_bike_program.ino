#include <SPI.h>
#include "vendor/rf24/nRF24L01.h"
#include "vendor/rf24/RF24.h"
#include <avr/io.h>
//#include "TimerOne.h"
#include "printf.h"
#include "bike_light_constants.h"

#include <SoftwareSerial.h>
SoftwareSerial mySerial(SERIAL_RX, SERIAL_TX); // RX, TX

// ISR interrupt service routine
#include <avr/interrupt.h>

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

// ============================================================================ //
// RADIO SETUP

RF24 radio(RADIO_PIN_CE,RADIO_PIN_CSN);

// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
//const uint64_t pipe = 0xF0F0F0F0AA;
// The various roles supported by this sketch
typedef enum { role_remote = 1, role_led } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
int retryCounter = 4;
role_e role;

// ============================================================================ //
// BUTTON PRESS HELPERS


boolean isModeSwitchState(unsigned int mode) {
  if ( mode >= LED_DEFAULT_STATE_ALL_OFF && mode < LED_TOTAL_DEFAULT_STATES) {
    return true;
  }
  return false;
}

void decrementModeState() {
    ledDefaultState--;
    if (!isModeSwitchState(ledDefaultState) ) {
       ledDefaultState = LED_DEFAULT_STATE_ANIMATION_1;
    }
}

void incrementModeState() {
    ledDefaultState++;
    if (!isModeSwitchState(ledDefaultState) ) {
       ledDefaultState = LED_DEFAULT_STATE_ALL_OFF;
    }
}

void modeButtonPress() {    
    printf("MODE BUTTON PRESS: ");
    printf("%d\n\r",ledDefaultState);
//  message[MESSAGE_LED_STATE] = currentLedState;
    message = ledDefaultState;
    shouldWriteNewState = true; 
}

void hornButtonPress() {    
    printf("HORN BUTTON PRESS: ");
    currentHornState = HORN_STATE_SIREN;
}

void leftButtonPress() {
//  Timer1.detachInterrupt();
  signalTimerExpired = false;
//  message[MESSAGE_LED_STATE] = LED_STATE_LEFT;
  message = LED_STATE_LEFT;
  shouldWriteNewState = true;
//  Timer1.attachInterrupt(signalExpired,SIGNAL_TIME_MILLI);
}

void rightButtonPress() {
//  Timer1.detachInterrupt();
  signalTimerExpired = false;
//  message[MESSAGE_LED_STATE] = LED_STATE_RIGHT;
  message = LED_STATE_RIGHT;
  shouldWriteNewState = true;

//  Timer1.attachInterrupt(signalExpired,SIGNAL_TIME_MILLI);
}

int hornClickCounter = 0;
int elapsedTimeSincePress = 0;
// ============================================================================ //
// OTHER HELPERS

void signalExpired(){
  printf("Signal Expired Timeout Fired");
  signalTimerExpired = true;
}

void checkAndUpdateButtonState() {
  int lightModePinState1 = digitalRead(MODE_PIN);

  if (lightModePinState1 == HIGH) {
    modeButtonPress();
    buttonPushCounter++;
    printf("number of button pushes: ");
    printf("%d\n\r",buttonPushCounter);
    delay(200); 
  }  

  if (digitalRead(HORN_IN_PIN) == HIGH) {
    printf("\nHORN PRESS %d\n", elapsedTimeSincePress);

    elapsedTimeSincePress+=STANDARD_DELAY_MILLI;
    if (HORN_STATE_CHIRP == currentHornState) {
          printf("\nHORN PRESS - turn off chirp\n");

      // if chirping, and you hit the horn, turn the horn off
      currentHornState = HORN_STATE_OFF;
      delay(200);
    } else if (HORN_STATE_SIREN == currentHornState || HORN_STATE_OFF == currentHornState && hornClickCounter == 0) {
                printf("\nHORN PRESS - HONKKKKK\n");

      // if you are off then honk
      hornButtonPress();
    }
  } else {

    int oldElapsedTime = elapsedTimeSincePress;
//    printf("\nHORN NOT PRESSED elapsedTime %d oldElapsedTime %d\n", elapsedTimeSincePress, oldElapsedTime);

    if (elapsedTimeSincePress > 0 && elapsedTimeSincePress < DOUBLE_CLICK_TIMER_MILLI) {

        hornClickCounter++;
        printf("HORN click counter++ %d \n", hornClickCounter);

        if (hornClickCounter==1) {
          printf("HORN single PRESS, ENTERING CHIRP MODE \n");
          doubleClickTimer = DOUBLE_CLICK_TIMER_MILLI;
        }
        if (hornClickCounter==2) {
          // second click
            printf("HORN DOUBLE PRESS, ENTERING CHIRP MODE \n");
            currentHornState = HORN_STATE_CHIRP;
            hornClickCounter = 0;
            doubleClickTimer = 0;
        }
    } else if (currentHornState == HORN_STATE_CHIRP) {
      // keep chirpin
    } else {
      // turn that shit off
      currentHornState = HORN_STATE_OFF;
    }
    elapsedTimeSincePress = 0;
  }
  
  if (doubleClickTimer>0) {
     printf("Double click time left: %d \n", doubleClickTimer);

     doubleClickTimer-= STANDARD_DELAY_MILLI;
  } else {
    doubleClickTimer = 0;
    hornClickCounter = 0;

  }
} 

// ============================================================================ //
// SETUP

void setup() {
  // set up the role pin
  role = role_remote;

//  printf_begin();

  mySerial.begin(4800);
  mySerial.println("Hello, This is the remote control...how are you?");
  mySerial.println("\n\rRF24/examples/pingpair/\n\r");
  char buf[50];
  sprintf(buf,"ROLE: %s\n\r",role_friendly_name[role]);
  mySerial.println(buf);

  radio.begin();
//  radio.setChannel(124);
  radio.setPALevel(RF24_PA_MAX);
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  // optionally, reduce the payload size.  seems to
  // improve reliability
//  radio.setPayloadSize(8);

  if ( role == role_remote )
  {
    radio.openWritingPipe(0xF0F0F0AA);

    pinMode(MODE_PIN, INPUT);
    pinMode(HORN_IN_PIN, INPUT);

    pinMode(HORN_OUT_PIN, OUTPUT);
    
    //numbers 0 (on digital pin 2) and 1 (on digital pin 3)
    attachInterrupt(0, leftButtonPress, FALLING);
    attachInterrupt(1, rightButtonPress, FALLING);
  }

  radio.printDetails();
  char *moreDetail = radio.getStats();
  mySerial.println(moreDetail);
  
  mySerial.println("Finished initialization"); 
}
 
void loop()
{
  // THE TRANSMITTER NODE
  //
  // Remote role.  If the state of any button has changed, send the whole state of
  // all buttons.
  //
//  int messageLen = sizeof(message[0])*num_messages;
  int messageLen = sizeof(message);
  
  if ( role == role_remote )
  {
    checkAndUpdateButtonState();
//    goto here;
    if (shouldWriteNewState == true) {
        mySerial.println("Now sending...");
        bool ok = radio.write( &message, messageLen );
        if (ok) {
          mySerial.println("ok\n\r");
          if (isModeSwitchState(ledDefaultState)) {
              incrementModeState();
          }
          shouldWriteNewState = false;
          retryCounter = 3;
          radio.powerDown();
        }
        else {
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
            shouldWriteNewState = false;
            retryCounter = 3;
            mySerial.println("failed\n\r");
          }
        }
    }
//here:
    // Try again in a short while
    if (currentHornState == HORN_STATE_SIREN) {
        digitalWrite(HORN_OUT_PIN,HIGH); 
    } else if (currentHornState == HORN_STATE_OFF) {
        digitalWrite(HORN_OUT_PIN,LOW); 
    } else if (currentHornState == HORN_STATE_CHIRP) {
      chirpTimer -= STANDARD_DELAY_MILLI;
      if (chirpTimer <= 0) {
        mySerial.println ("CHIRP\n");
        digitalWrite(HORN_OUT_PIN,HIGH); 
        delay(30);
        digitalWrite(HORN_OUT_PIN,LOW); 
        chirpTimer = CHIRP_TIMER_MILLI;
      } else {
        digitalWrite(HORN_OUT_PIN,LOW); 
      }
    }
    delay(STANDARD_DELAY_MILLI);

 
  } // end remote role
  
}

// TURN ON: 1,3,5,6,7,8,9
// TURN OFF: 2,4
void toggleLEDS(int ledBitmask) {
  
  for(int i = 0; i < numRows; i++){ 
    int bitTest = 1 << i;
    if (bitTest & ledBitmask) {
        if (i == numRows-1) {
          digitalWrite(LED_9_PIN, HIGH);
        } else {
          setRegisterPin(ledPins[i], HIGH); 
        }
    } else {
        if (i == numRows-1) {
          digitalWrite(LED_9_PIN, LOW);
        } else {
          setRegisterPin(ledPins[i], LOW); 
        }
    }
  } 
  writeRegisters();
}
 
void allOff () {
//  mySerial.println("ALL OFF HITTTTT");
  digitalWrite(LED_9_PIN, LOW);
  clearRegisters();
  writeRegisters();
} 

void allOn () {
  for (int i=0; i<numRows; i++) {
      if (i == numRows-1) {
        digitalWrite(LED_9_PIN, HIGH);
      } else {
        setRegisterPin(ledPins[i], HIGH); 
      }
  }
  writeRegisters();
} 

//set all register pins to LOW
void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
} 

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value){
  registers[index] = value;
}

void writeRegisters(){

  digitalWrite(RCLK_Pin, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);

  }
  
  digitalWrite(RCLK_Pin, HIGH); 
}

