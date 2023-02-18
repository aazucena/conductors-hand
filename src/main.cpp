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
 * @version 0.1
 * @date 2023-02-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>              // Arduino Library
#include <string.h>
#include <math.h>                // C+ Math Library 
#include <MozziGuts.h>           // Main mozzi object

/// Flex Pins based on Fingers
const int thumb_flex_pin = A0;
const int index_flex_pin = A1;
const int middle_flex_pin = A2;
const int ring_flex_pin = A3;
const int pinky_flex_pin = A4;

/// Flex Pin based on your wrist
const int wrist_flex_pin = A5;

/// LED Pins based on Fingers
const int thumb_led_pin = 3;
const int index_led_pin = 4;
const int middle_led_pin = 5;
const int ring_led_pin = 6;
const int pinky_led_pin = 7;

// Change these constants according to your project's design
const float vcc = 5;			// voltage at Ardunio 5V line
const float resist_divider = 10000.0;	// resistor used to create a voltage divider
const float flat_resistance = 29960.94;	// resistance when flat
const float bend_resistance = 163389.83;	// resistance at 180 deg

void setup() {
	pinMode(thumb_led_pin, OUTPUT);
	pinMode(index_led_pin, OUTPUT);
	pinMode(middle_led_pin, OUTPUT);
	pinMode(ring_led_pin, OUTPUT);
	pinMode(pinky_led_pin, OUTPUT);
  
	pinMode(thumb_flex_pin, INPUT);
	pinMode(index_flex_pin, INPUT);
	pinMode(middle_flex_pin, INPUT);
	pinMode(ring_flex_pin, INPUT);
	pinMode(pinky_flex_pin, INPUT);

	pinMode(wrist_flex_pin, INPUT);
  startMozzi(); //initializes the Mozzi Library for synth generation
}


/**
 * @brief Observes the controller's actions based on the synth
 *
 */
void updateControl() {
  /// insert code that updates the frequency
	detectFingerFlex(thumb_flex_pin, thumb_led_pin, "Thumb");
	detectFingerFlex(index_flex_pin, index_led_pin, "Index");
	detectFingerFlex(middle_flex_pin, middle_led_pin, "Middle");
	detectFingerFlex(ring_flex_pin, ring_led_pin, "Ring");
	detectFingerFlex(pinky_flex_pin, pinky_led_pin, "Pinky");
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


void detectFingerFlex(int flex_pin, int led_pin, String finger_name) {
	// Read the ADC (Analog to DC), and calculate voltage and resistance from it
	int adc = analogRead(flex_pin);
	float voltage = adc * vcc / 1023.0;
	float resistance = resist_divider * (vcc / voltage - 1.0);
  
  Serial.println("**" + finger_name + " Finger**");
	Serial.println("Resistance: " + String(resistance) + " ohms");

	// Use the calculated resistance to estimate the sensor's bend angle:
	float angle = map(resistance, flat_resistance, bend_resistance, 0, 180.0);
  
	Serial.println("Bend: " + String(angle) + " degrees");
	Serial.println();
  
  if (angle > 45.0) {
    analogWrite(led_pin, 255);
  } else {
    analogWrite(led_pin, 0);
  }
}