# Conductor's Hand

## Team
### Name
  - Group #3
### Members
  - Aldrin Azucena
  - Sophia Mai
  - Xavier Brooks



## Description
The project is about building a glove device where each of your five fingers will play different notes and light up each of the LEDs while moving your wrist changes the octave of the sound. This device involves the movement of your lower arm which includes your wrist, hand, and fingers to generate the sound.

## Design Process
### Iterations
#### 1
![Iteration_1](iterations/iteration1.png)
#### 2 & 3
![Iteration_2_and_3](iterations/iteration2-3.png)



### Pseudocode
```c++
/// Initialize the glove

/// If one of the fingers (0-5) have either moved up or down
	/// Set the designated frequency for that finger 
	/// If the wrist moves up
		/// Set to the upper octave
	/// Else if the wrist moves down
		/// Set to the lower octave
	/// Else
		/// Set to the current octave
	/// Play Sound
	/// Light the Designated LED
/// Else if two or more fingers either move up or down
	/// Set the designated frequencies for the detected fingers
	/// If the wrist moves up
		/// Set to the upper octave
	/// Else if the wrist moves down
		/// Set to the lower octave
	/// Else
		/// Set to the current octave
	/// Play Sound
	/// Light the Designated LEDs
/// END
```

### Components Parts
- Gloves (2x)
- KT Tape
- Copper Wires
- Graphite Colored Paper (5x)
- 3D Printed Wrist Gauntlet
- LEDs (4x)
- Aux Speaker Pin
- 5 Flex Sensors
- Electrical Terminals
- Velcro (Fasteners)
- M5 Screws
- Nuts & Washers


## Tasks Done So Far
- Aldrin Azucena
	- Created 5 Homemade Flex Sensors
	- Experiment with Sound Synths on the Arduino Code
	- Bought materials for the project ($18)
		- Aluminum Foil
		- KT Tape
  - Composed the Design Process Documentation
- Xavier Brooks
  - Powerpoint Presentation
- Sophia Mai
	- Design Iterations & Sketches
	- Bought 3 Pairs of Gloves ($5)

## TODO
- Build The glove
  - Flex Sensors
  - LEDs
  - 3.5 Audio Jack
- Develop the code
  - Sound Synth
  - LED system
- 3D Print the project based on these design inspirations
  - [Arduino Holder](https://www.thingiverse.com/thing:1252601)
  - [Forearm Split 1](https://www.thingiverse.com/thing:3613626/files)
  - [Strap Adjuster](https://www.thingiverse.com/thing:1777669)
  - [Arduino Top Case](https://www.thingiverse.com/thing:994827/files)
  - [Hinges](https://www.thingiverse.com/thing:4924410)