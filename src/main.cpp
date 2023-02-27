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
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#define AUDIO_MODE STANDARD_PLUS // Set Audio mode to Standard Plus

/// Total number of fingers
const int total_fingers = 5;

/// Control Rate of the Audio Output
#define CONTROL_RATE 64


enum Finger {
  THUMB = 0,
  INDEX = 1,
  MIDDLE = 2,
  RING = 3,
  PINKY = 4
};

/// Oscillators based on fingers
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aOscils[total_fingers] = {
  (SIN2048_DATA),
  (SIN2048_DATA),
  (SIN2048_DATA),
  (SIN2048_DATA),
  (SIN2048_DATA),
};

/// for triggering the envelope
EventDelay noteDelay;
EventDelay noteDelays[total_fingers];

/// ADSR Envelope for note dynmaics
ADSR <CONTROL_RATE, AUDIO_RATE> envelopes[total_fingers];

/// Default ADSR constant values 
const unsigned int ATTACK = 12;
const unsigned int DECAY = 50;
const unsigned int SUSTAIN = 60000;
const unsigned int RELEASE_MS = 40;
const byte ATTACK_LEVEL = 255;
const byte DECAY_LEVEL = 255;

/// ADSR values for the fingers
unsigned int attack[total_fingers] = { 
  ATTACK,
  ATTACK,
  ATTACK,
  ATTACK,
  ATTACK,
};
unsigned int decay[total_fingers] = { 
  DECAY,
  DECAY,
  DECAY,
  DECAY,
  DECAY,
};
unsigned int sustain[total_fingers] = { 
  SUSTAIN,
  SUSTAIN,
  SUSTAIN,
  SUSTAIN,
  SUSTAIN,
};
unsigned int release_ms[total_fingers] = { 
  RELEASE_MS,
  RELEASE_MS,
  RELEASE_MS,
  RELEASE_MS,
  RELEASE_MS,
};

byte attack_level[total_fingers] = { 
  ATTACK_LEVEL,
  ATTACK_LEVEL,
  ATTACK_LEVEL,
  ATTACK_LEVEL,
  ATTACK_LEVEL,
};
byte decay_level[total_fingers] = { 
  DECAY_LEVEL,
  DECAY_LEVEL,
  DECAY_LEVEL,
  DECAY_LEVEL,
  DECAY_LEVEL,
};



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

/// Boolean Array to check if  the note is triggered
boolean is_note_on[total_fingers];

/**
 * @brief Sets the ADSR envelope whenever it plays the note
 * 
 * @param finger 
 */
void setADSREnvelope(Finger finger) {
  /// Set levels for the attack and decay
  envelopes[finger].setADLevels(attack_level[finger],decay_level[finger]);
  
  /// Set level for release
  envelopes[finger].setReleaseLevel(0);

  /// Set the envelope times for ADSR
  envelopes[finger].setTimes(attack[finger], decay[finger], sustain[finger], release_ms[finger]);
}
/**
 * @brief Set the ADSR envelope with the new  values
 * 
 * @param finger 
 * @param atk 
 * @param dec 
 * @param sus 
 * @param rel 
 * @param atk_lvl 
 * @param dec_lvl 
 */
void changeADSREnvelope(
  Finger finger, 
  unsigned int atk,
  unsigned int dec,
  unsigned int sus,
  unsigned int rel,
  byte atk_lvl,
  byte dec_lvl
) {
  attack[finger] = atk;
  decay[finger] = dec;
  sustain[finger] = sus;
  release_ms[finger] = rel;
  attack_level[finger] = atk_lvl;
  decay_level[finger] = dec_lvl;
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
 * @ref https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
 * @param finger 
 * @return float 
 */
float getNote(Finger finger) {
  switch(finger) {
    case THUMB:
      return mtof(65) * wrist_mod;
      // return 349.f * wrist_mod;
    case INDEX:
      return mtof(69) * wrist_mod;
      // return 440.f * wrist_mod;
    case MIDDLE:
      return mtof(72) * wrist_mod;
      // return 523.f * wrist_mod;
    case RING:
      return mtof(76) * wrist_mod;
      // return 659.f * wrist_mod;
    case PINKY:
      return mtof(79) * wrist_mod;
      // return 784.f * wrist_mod;
    default:
      return 0;
  }
}

/**
 * @brief Change the ADSR values based on the finger and the angle of the flex sensor
 * 
 * @param finger 
 * @param angle 
 */
void setGain(Finger finger, float angle) {
  if (angle >= 45.0 && angle < 56.25) {

    changeADSREnvelope(finger, ATTACK*0.25, DECAY*0.25, SUSTAIN*0.25, RELEASE_MS*0.25, ATTACK_LEVEL, DECAY_LEVEL);
  } else if (angle >= 56.25 && angle < 67.5) {

    changeADSREnvelope(finger, ATTACK*0.5, DECAY*0.5, SUSTAIN*0.5, RELEASE_MS*0.5, ATTACK_LEVEL, DECAY_LEVEL);
  } else if (angle >= 67.5) {
    
    changeADSREnvelope(finger, ATTACK, DECAY, SUSTAIN, RELEASE_MS, ATTACK_LEVEL, DECAY_LEVEL);
  } else {
    changeADSREnvelope(finger, 0, 0, 0, 0, 0, 0);
  }
}


/**
 * @brief Detects the finger from the designated flex sensor
 * 
 * @param finger 
 */
void detectFingerFlex(Finger finger) {
  int flex_pin = flex_pins[finger];
  int led_pin = led_pins[finger];
  float freq = getNote(finger);
  String finger_name = getFingerName(finger);

  // Read the ADC (Analog to DC), and calculate voltage and resistance from it
  int adc = mozziAnalogRead(flex_pin);
  float voltage = adc * vcc / 1023.0;
  float resistance = resist_divider * (vcc / voltage - 1.0);

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(resistance, flat_resistance, bend_resistance, 0, 90.0);
  if (angle >= 45.0)
  {
    setGain(finger, angle);
    envelopes[finger].noteOn();
    is_note_on[finger] = true;
    (aOscils[finger]).setFreq(freq);
    envelopes[finger].update();
    analogWrite(led_pin, 255);
  }
  else
  {
    changeADSREnvelope(finger, ATTACK, DECAY, SUSTAIN, RELEASE_MS, ATTACK_LEVEL, DECAY_LEVEL);
    envelopes[finger].noteOff();
    envelopes[finger].update();
    (aOscils[finger]).setFreq(0);
    is_note_on[finger] = false;
    analogWrite(led_pin, 0);
  }
  setADSREnvelope(finger);
  noteDelays[finger].start(attack[finger]+decay[finger]+sustain[finger]+release_ms[finger]);
}

/**
 * @brief Detects the wrist from the designated flex sensor
 * 
 * @param flex_pin 
 */
void detectWristFlex(int flex_pin) {
  
  // Read the ADC (Analog to DC), and calculate voltage and resistance from it
  int adc = analogRead(flex_pin);
  float voltage = adc * vcc / 1023.0;
  float resistance = resist_divider * (vcc / voltage - 1.0);

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(resistance, flat_resistance, bend_resistance, 0, 90.0);
  // if (angle >= 45.0) {
  //   wrist_mod = 0.5;
  // } else {
  //   wrist_mod = 1;
  // }
}


void setup() {
  for(int i = 0; i < total_fingers; i++) {
    pinMode(led_pins[i], OUTPUT);
    pinMode(flex_pins[i], INPUT);
  }

  pinMode(wrist_flex_pin, INPUT);
  // Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  /// Set Serial Code on 11520 to prevent any interruptions from different serial ports 
  Serial.begin(115200); 
  randSeed(); // fresh random
  /// Set ADSR Envelopes for the fingers
  setADSREnvelope(THUMB);
  setADSREnvelope(INDEX);
  setADSREnvelope(MIDDLE);
  setADSREnvelope(RING);
  setADSREnvelope(PINKY);

  /// Set note delays for each fingers by 2000ms
  for(int i = 0; i < total_fingers; i++) {
    noteDelays[i].set(2000); // 2 second countdown
  }
  // noteDelay.set(2000); // 2 second countdown
  startMozzi(CONTROL_RATE); // initializes the Mozzi Library for synth generation
}

/**
 * @brief Observes the controller's actions based on the synth
 *
 */
void updateControl() {

  /// Get Pitch by each finger detection
  detectWristFlex(wrist_flex_pin);

  /// Get Note by the each finger detection
  detectFingerFlex(THUMB);
  detectFingerFlex(INDEX);
  detectFingerFlex(MIDDLE);
  detectFingerFlex(RING);
  detectFingerFlex(PINKY);
  
  /// Check if the note is on for each finger
  for (int i = 0; i < total_fingers; i++) {
    if (is_note_on[i] == true) {
      envelopes[i].update();
    }
  }
}

/**
 * @brief Updates the audio frequency code that should return the integer between -244 to 243
 *
 * @return int
 */
int updateAudio() {
  
  /// Play the audio based from each notes from the finger
  long result = 0;

  const int byte_shift = 10;
  
  for (int i = 0; i < 4; i++) {
    if (is_note_on[i] == true) {
      result += ((int)aOscils[i].next() * envelopes[i].next());
    } else {
      result += 0;
    }
  }
  return (result) >> byte_shift; // insert frequency here
}

void loop() {
  audioHook(); // Run the audio hook to run the synth
}
