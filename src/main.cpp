/**
 * @file conductors-hand.ino
 * @author Aldrin Azucena (a.azucena@uleth.ca)
 * @author Xavier Brooks (xavier.brooks@uleth.ca)
 * @author Sophia Mai (sophia.mai@uleth.ca)
 * @brief The program that runs the LED system and a synth generator
 * @ref https://sensorium.github.io/Mozzi/examples
 * @version 0.1
 * @date 2023-02-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>              // Arduino Library
#include <math.h>                // C+ Math Library 
#include <MozziGuts.h>           // Main mozzi object


void setup() {
    startMozzi(); //initializes the Mozzi Library for synth generation
}


/**
 * @brief Observes the controller's actions based on the synth
 *
 */
void updateControl() {
  /// insert code that updates the frequency
}

/**
 * @brief Updates the audio frequency code that should return the integer between -244 to 243
 *
 * @return int
 */
int updateAudio() {
  return 0; // insert frequency here
}

void loop() {
  audioHook(); // Run the audio hook to run the synth
}