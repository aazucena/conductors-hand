/**
 * @file conductors-hand.ino
 * @author Aldrin Azucena (a.azucena@uleth.ca)
 * @author Xavier Brooks (xavier.brooks@uleth.ca)
 * @author Sophia Mai (sophia.mai@uleth.ca)
 * @brief The program that runs the LED system and a synth generator
 * @ref https://sensorium.github.io/Mozzi/examples
 * @ref https://learn.sparkfun.com/tutorials/flex-sensor-hookup-guide/example-program
 * @ref https://www.instructables.com/Arduino-Flex-Sensor-Glove/
 * @ref https://github.com/JonathanCates/Flex_Sensor_Glove/blob/master/Flex_Sensor_Glove.ino
 * @ref https://lastminuteengineers.com/flex-sensor-arduino-tutorial/#:~:text=The%20resistance%20of%20the%20flex,Rflex)%20%2B%20%22%20ohms%22)%3B
 * @ref https://circuitdigest.com/microcontroller-projects/interfacing-flex-sensor-with-arduino
 * @ref https://quod.lib.umich.edu/cgi/p/pod/dod-idx/mozzi-interactive-sound-synthesis-on-the-open-source-arduino.pdf?c=icmc;idno=bbp2372.2013.002
 * @version 0.1
 * @date 2023-02-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <Arduino.h> // Arduino Library
#include <string.h>
#include <math.h>      // C+ Math Library
#include <MozziGuts.h> // Main mozzi object
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin2048_int8.h> 
#include <Smooth.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 128

const int total_fingers = 5;


enum Finger {
  THUMB = 0,
  INDEX = 1,
  MIDDLE = 2,
  RING = 3,
  PINKY = 4
};

/// Oscillators based on fingers
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aOscils[total_fingers] = {
  Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> (SIN2048_DATA),
  Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> (SIN2048_DATA),
  Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> (SIN2048_DATA),
  Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> (SIN2048_DATA),
  Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> (SIN2048_DATA),
};

Smooth <int> kSmoothFreq(0.975f);
/// for triggering the envelope
EventDelay noteDelay;

ADSR <CONTROL_RATE, AUDIO_RATE> envelopes[total_fingers];
unsigned int duration, attack, decay, sustain, release_ms;


/// Flex Pins based on Fingers
const int flex_pins[total_fingers] = {
  A0, // Thumb
  A1, // Index
  A2, // Middle
  A3, // Ring
  A4 // Pinky
};

/// Flex Pin based on your wrist
const int wrist_flex_pin = A5;

/// LED Pins based on Fingers
const int led_pins[total_fingers] = {
  3, // Thumb
  4, // Index
  5, // Middle
  6, // Ring
  7 // Pinky
};
/// Change these constants according to your project's design
const float vcc = 5;                     // voltage at Ardunio 5V line
const float resist_divider = 10000.0;    // resistor used to create a voltage divider
const float flat_resistance = 24677.96;  // resistance when flat
const float bend_resistance = 67500.00; // resistance at 90 deg

/// pitch mod from wrist
int wrist_mod = 1;


/**
 * @brief SEts the ADSR envelope whenever it plays the note
 * 
 * @param attack_lvl 
 * @param decay_lvl 
 */
void setADSREnvelope(Finger finger) {

  byte attack_level = rand(128)+127;
  byte decay_level = rand(255);
  envelopes[finger].setADLevels(attack_level,decay_level);
  envelopes[finger].setReleaseLevel(0);
  attack = 50;
  decay = 200;
  release_ms = 200;
  sustain = 200;
  envelopes[finger].setTimes(attack, decay, sustain, release_ms);
}


/**
 * @brief Get the Finger Name string based on the enum Finger object
 * 
 * @param finger 
 * @return String 
 */
String getFingerName(Finger finger) {
  switch(finger) {
    case THUMB:
      return "Thumb";
    case INDEX:
      return "Index";
    case MIDDLE:
      return "Middle";
    case RING:
      return "Ring";
    case PINKY:
      return "Pinky";
    default:
      return "";
  }
}

/**
 * @brief Get the Note frequency based on the finger flex pin
 * F Ionian Major Scale
 * Thumb = F4
 * Index = A4
 * Middle = C5
 * Ring = E5
 * Pinky = G5 
 * @ref https://pages.mtu.edu/~suits/notefreqs.html
 * @param finger 
 * @return float 
 */
float getNote(Finger finger) {
  switch(finger) {
    case THUMB:
      return 349.f * wrist_mod;
    case INDEX:
      return 440.f * wrist_mod;
    case MIDDLE:
      return 523.f * wrist_mod;
    case RING:
      return 659.f * wrist_mod;
    case PINKY:
      return 784.f * wrist_mod;
    default:
      return 0;
  }
}

/**
 * @brief Detects the finger from the designated flex sensor
 * 
 * @param finger 
 */
void detectFingerFlex(Finger finger) {
  setADSREnvelope(finger);
  int flex_pin = flex_pins[finger];
  int led_pin = led_pins[finger];
  float freq = getNote(finger);
  String finger_name = getFingerName(finger);

  // Read the ADC (Analog to DC), and calculate voltage and resistance from it
  int adc = mozziAnalogRead(flex_pin);
  float voltage = adc * vcc / 1023.0;
  float resistance = resist_divider * (vcc / voltage - 1.0);

  Serial.println("**" + finger_name + " Finger**");
  Serial.println("Resistance: " + String(resistance) + " ohms");

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(resistance, flat_resistance, bend_resistance, 0, 90.0);

  Serial.println("Bend: " + String(abs(angle)) + " degrees");
  Serial.println();

  if (abs(angle) > 45.0)
  {
    envelopes[finger].noteOn();
    int smoothed_freq = kSmoothFreq.next(freq);
    (aOscils[finger]).setFreq(smoothed_freq);
    envelopes[finger].update();
    analogWrite(led_pin, 255);
  }
  else
  {
    envelopes[finger].noteOff();
    envelopes[finger].update();
    analogWrite(led_pin, 0);
  }
}

/**
 * @brief Detects the wrist from the designated flex sensor
 * 
 * @param flex_pin 
 */
void detectWristFlex(int flex_pin) {
  
  // Read the ADC (Analog to DC), and calculate voltage and resistance from it
  int adc = mozziAnalogRead(flex_pin);
  float voltage = adc * vcc / 1023.0;
  float resistance = resist_divider * (vcc / voltage - 1.0);

  Serial.println("**Wrist**");
  Serial.println("Resistance: " + String(resistance) + " ohms");

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(resistance, flat_resistance, bend_resistance, 0, 90.0);

  Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println();
}


void setup() {
  for(int i = 0; i < total_fingers; i++) {
    pinMode(led_pins[i], OUTPUT);
    pinMode(flex_pins[i], INPUT);
  }

  pinMode(wrist_flex_pin, INPUT);
  // Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  randSeed(); // fresh random
  
  setADSREnvelope(THUMB);
  setADSREnvelope(INDEX);
  setADSREnvelope(MIDDLE);
  setADSREnvelope(RING);
  setADSREnvelope(PINKY);
  noteDelay.set(2000); // 2 second countdown
  startMozzi(CONTROL_RATE); // initializes the Mozzi Library for synth generation
}

/**
 * @brief Observes the controller's actions based on the synth
 *
 */
void updateControl() {
  /// insert code that updates the frequency

  if(noteDelay.ready()) {
    detectFingerFlex(THUMB);
    // delay(500);
    detectFingerFlex(INDEX);
    // delay(500);
    detectFingerFlex(MIDDLE);
    detectFingerFlex(RING);
    detectFingerFlex(PINKY);
  }
  for (int i = 0; i < total_fingers; i++) {
    envelopes[i].update();
  }
}

/**
 * @brief Updates the audio frequency code that should return the integer between -244 to 243
 *
 * @return int
 */
int updateAudio() {
  long result = 0;
  for (int i = 0; i < total_fingers; i++) {
    if (i == 0) {
      result += (int)(envelopes[i].next() * aOscils[i].next());
    } else {
      result += (int)(envelopes[i].next() * aOscils[i].next());
    }
  }
  return (result) >> 8; // insert frequency here
}

void loop() {
  audioHook(); // Run the audio hook to run the synth
}
