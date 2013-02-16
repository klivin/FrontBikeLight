
/**
 * @file bike_light_constants.h
 *
 * Defines state and enumeration for bike light program 
 */
 
#ifndef __BIKE_LIGHT_CONSTANTS_H__
#define __BIKE_LIGHT_CONSTANTS_H__

enum LedState {
  LED_STATE_RIGHT=99,
  LED_STATE_LEFT=100,
  LED_STATE_ALL_OFF=101,
  LED_STATE_ALL_ON=102,
  LED_STATE_ANIMATION_1=103,
  LED_TOTAL_STATES=104
};

enum LedDefaultState {
  LED_DEFAULT_STATE_ALL_OFF=LED_STATE_ALL_OFF,
  LED_DEFAULT_STATE_ALL_ON=LED_STATE_ALL_ON,
  LED_DEFAULT_STATE_ANIMATION_1=LED_STATE_ANIMATION_1,
  LED_TOTAL_DEFAULT_STATES=LED_TOTAL_STATES
};

enum RadioState {
   RADIO_STATE_OFF,
   RADIO_STATE_ON,
   RADIO_STATE_LOW_POWER
};

enum HornState {
   HORN_STATE_OFF,
   HORN_STATE_SIREN,
   HORN_STATE_CHIRP,
   HORN_TOTAL_STATES
};

// ============================================================================ //
// INITIAL CONFIGURATION

int buttonPushCounter = 0;   // counter for the number of button presses

const unsigned long SIGNAL_TIME_MILLI = 3000000; // 3 seconds
const unsigned long CHIRP_TIMER_MILLI = 4000000; // 4 seconds
const unsigned long DOUBLE_CLICK_TIMER_MILLI = 200; // .2 second

const unsigned long STANDARD_DELAY_MILLI = 20;
volatile unsigned int currentLedState = LED_STATE_RIGHT;
volatile unsigned int oldLedState = LED_STATE_LEFT;
volatile unsigned int ledDefaultState = LED_DEFAULT_STATE_ALL_OFF;

volatile boolean shouldWriteNewState = false;
volatile boolean signalTimerExpired = true;
static int numRows = 9;
static int currentHornState = HORN_STATE_OFF;
static int chirpTimer = CHIRP_TIMER_MILLI;
static int doubleClickTimer = 0;

// ============================================================================ //
// PINS SETUP
// need to make this pin different for the remote side
const int LED_9_PIN      = 12;
// Need to make this pin different for the remote side

const int SERIAL_TX      = 7;
const int SERIAL_RX      = 3;
const int MODE_PIN       = 4;
const int HORN_IN_PIN    = 6;
const int HORN_OUT_PIN   = 8;

const int RADIO_PIN_CE   = 9;   // pin 3 on nRF24L01
const int RADIO_PIN_CSN  = 10;  // pin 4 on nRF24L01
const int RADIO_PIN_MOSI = 11;  // ??? pin 6 on nRF24L01
const int RADIO_PIN_MISO = 7;   // ??? pin 7 on nRF24L01
const int RADIO_PIN_SCK  = 13;  // ??? pin 5 on nRF24L01

int SER_Pin = 1;   //pin 14 on the 75HC595
int RCLK_Pin = 4;  //pin 12 on the 75HC595
int SRCLK_Pin = 5; //pin 11 on the 75HC595


const int num_messages = 2;
const int MESSAGE_LED_STATE = 0;
const int MESSAGE_MODE_STATE = 1;
//unsigned int message[num_messages];
unsigned int message;

// ============================================================================ //
// LED SETUP

int ledPins[] = {0,1,2,3,4,5,6,7}; 

// Bitmasks for easy setting state of pins
const unsigned int LEFT_SIGNAL_MASK = 0b111110101;
//8,7,0,1,2,3,4,5,6,
//const unsigned int LEFT_SIGNAL_MASK = 0b111100000;
const unsigned int RIGHT_SIGNAL_MASK  = 0b101011111;

const unsigned int ANIMATION_1 = 0b100000000;
const unsigned int ANIMATION_2 = 0b010000000;
const unsigned int ANIMATION_3 = 0b000100000;
const unsigned int ANIMATION_4 = 0b000001000;
const unsigned int ANIMATION_5 = 0b000000010;
const unsigned int ANIMATION_6 = 0b000000001;
const unsigned int animation1Array[6] = {ANIMATION_1,ANIMATION_2,ANIMATION_3,ANIMATION_4,ANIMATION_5,ANIMATION_6};

// ============================================================================ //
// SHIFT REGISTER SETUP



//How many of the shift registers - change this
#define number_of_74hc595s 1 

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8
boolean registers[numOfRegisterPins];
              
#endif
