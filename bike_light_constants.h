
/**
 * @file bike_light_constants.h
 *
 * Defines state and enumeration for bike light program 
 */
 
#ifndef __BIKE_LIGHT_CONSTANTS_H__
#define __BIKE_LIGHT_CONSTANTS_H__

typedef enum ButtonPress {
  BUTTON_PRESS_NONE,
  BUTTON_PRESS_LEFT,
  BUTTON_PRESS_RIGHT,
  BUTTON_PRESS_MODE,
  BUTTON_PRESS_CANCEL  
};

typedef enum LedState {
  LED_STATE_NONE=98,
  LED_STATE_RIGHT, //99
  LED_STATE_LEFT, //100
  LED_STATE_ALL_OFF, //101
  LED_STATE_ALL_ON, //102
  LED_STATE_PIN_WHEEL, //103
  LED_STATE_BLINK, //104
  LED_TOTAL_STATES //105
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
int currentTimerID = 0;
const unsigned long SIGNAL_TIME_MILLI = 3000000; // 3 seconds
const unsigned long CHIRP_TIMER_MILLI = 4000000; // 4 seconds
const unsigned long DOUBLE_CLICK_TIMER_MILLI = 20000; // .2 second
const unsigned long DELAY_FOR_MODEM_SIGNAL = 100000; // .2 second

const unsigned long STANDARD_DELAY_MILLI = 20;
volatile unsigned int oldLedState = LED_STATE_LEFT;
//volatile unsigned int ledDefaultState = LED_STATE_RIGHT;
volatile unsigned int ledDefaultState = LED_STATE_BLINK;
volatile unsigned int currentLedState = ledDefaultState;


volatile boolean shouldWriteNewState = false;
volatile boolean signalTimerExpired = true;
static int numRows = 9;
static int currentHornState = HORN_STATE_OFF;
static int chirpTimer = CHIRP_TIMER_MILLI;
static int doubleClickTimer = 0;

// ============================================================================ //
// PINS SETUP


const int TLC_GSLCLK_PIN = 3;
const int TEST_PIN_5     = 5;
const int AUDIO_MOD_PIN  = 4;   // THIS IS PIN 4 on the atmega
const int RADIO_PIN_CE   = 7;   // pin 3 on nRF24L01
const int RADIO_PIN_CSN  = 8;  // pin 4 on nRF24L01
const int XLAT_PIN       = 9;  // pin 
const int BLANK_PIN      = 10;  // pin 
const int RADIO_PIN_MOSI = 12;  // ??? pin 6 on nRF24L01
const int RADIO_PIN_MISO = 11;  // ??? pin 7 on nRF24L01
const int RADIO_PIN_SCK  = 13;  // ??? pin 5 on nRF24L01

// Vcc  Mosi  GND
// Miso  SCK  RESET

// BUTTON PINS
const int LEFT_PIN       = 0;
const int MODE_PIN       = 1;//1;
const int RIGHT_PIN      = 2;//2;
const int CANCEL_PIN     = 4;


const int HORN_OUT_PIN   = 8;

// SOFTWARE SERIAL
const int SERIAL_RX      = 6;
const int SERIAL_TX      = TEST_PIN_5;

const int NO_TIMER = 999;

const int num_messages = 2;
const int MESSAGE_LED_STATE = 0;
const int MESSAGE_MODE_STATE = 1;
//unsigned int message[num_messages];
unsigned int message;

// ============================================================================ //
// LED SETUP
const int NUM_LEDS = 15;
//TODO: Try this out by doing a mapping from pin to the 
// INVERT then reorder
// For the bike light
//        8    7
//    9           6
// 13   10   5  4    1
//    12          2
//        11   3
// 0                  14
// Old Proto
//        1    2
//    3            4
// 6     5   7   8    9       
//    10          11
//        12   13
const int mapChannelIndexToLEDPosition[NUM_LEDS] = { 0,8,7,9,6,13,10,5,4,1,12,2,11,3,14  };
//const int mapChannelIndexToLEDPosition[15] = { 0,1,8,2,11,3,4,5,6,7,10,12,13,9  };
// For the test setup
//const int mapChannelIndexToLEDPosition[16] = { 0,1,2,3,4,5,6,7,8,10,9,11,12,13,14 };
const int maxChannelIndex =  NUM_LEDS-1;

const int LEFT_ARROW[NUM_LEDS] = { 0,9,0,9,0,9,9,9,9,9,9,0,9,0 };
const int RIGHT_ARROW[NUM_LEDS] = { 0,0,9,0,9,9,9,9,9,9,0,9,0,9 };

const int ANN_2aPIN_1[NUM_LEDS] = { 0,9,0,0,0,0,0,0,0,0,0,0,0,0 };
const int ANN_2aPIN_2[NUM_LEDS] = { 0,0,9,0,0,0,0,1,0,0,0,0,0,0 };
const int ANN_2aPIN_3[NUM_LEDS] = { 0,0,0,0,9,0,0,2,0,0,0,0,0,0 };
const int ANN_2aPIN_4[NUM_LEDS] = { 0,0,0,0,0,0,0,4,9,9,0,0,0,0 };
const int ANN_2aPIN_5[NUM_LEDS] = { 0,0,0,0,0,0,0,9,0,0,0,9,0,0 };
const int ANN_2aPIN_6[NUM_LEDS] = { 0,0,0,0,0,0,0,9,0,0,0,0,0,9 };
const int ANN_2aPIN_7[NUM_LEDS] = { 0,0,0,0,0,0,0,4,0,0,0,0,9,0 };
const int ANN_2aPIN_8[NUM_LEDS] = { 0,0,0,0,0,0,0,2,0,0,9,0,0,0 };
const int ANN_2aPIN_9[NUM_LEDS] = { 0,0,0,0,0,9,9,1,0,0,0,0,0,0 };
const int ANN_2aPIN_10[NUM_LEDS] = { 0,0,0,9,0,0,0,0,0,0,0,0,0,0 };
const int *CLOCK_PULSE_ANN[10] = {ANN_2aPIN_2,ANN_2aPIN_3,ANN_2aPIN_4,ANN_2aPIN_5,
                               ANN_2aPIN_6,ANN_2aPIN_7,ANN_2aPIN_8,ANN_2aPIN_9,ANN_2aPIN_10,ANN_2aPIN_1};   

const int A1n[NUM_LEDS] = { 0,9,9,9,9,9,0,0,0,9,9,9,9,9 };
const int A2n[NUM_LEDS] = { 0,8,8,8,8,8,1,1,1,8,8,8,8,8 };
const int A3n[NUM_LEDS] = { 0,7,7,7,7,7,2,2,2,7,7,7,7,7 };
const int A4n[NUM_LEDS] = { 0,6,6,6,6,6,3,3,3,6,6,6,6,6 };
const int A5n[NUM_LEDS] = { 0,5,5,5,5,5,3,3,3,5,5,5,5,5 };
const int A6n[NUM_LEDS] = { 0,4,4,4,4,4,4,4,4,4,4,4,4,4 };
const int A7n[NUM_LEDS] = { 0,3,3,3,3,3,5,5,5,3,3,3,3,3 };
const int A8n[NUM_LEDS] = { 0,2,2,2,2,2,6,6,6,2,2,2,2,2 };
const int A9n[NUM_LEDS] = { 0,1,1,1,1,1,7,7,7,1,1,1,1,1 };
const int A10n[NUM_LEDS] = {0,0,0,0,0,0,8,8,8,0,0,0,0,0 };
const int A11n[NUM_LEDS] = { 0,0,0,0,0,0,9,9,9,0,0,0,0,0 };
const int *BLINK_FADE_ANN[11] = {A1n,A2n,A3n,A4n,A5n,A6n,A7n,A8n,A9n,A10n,A11n};        
              
#endif



