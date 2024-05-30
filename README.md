# Laser Cut Machine Control System

## Introduction
This repository contains the implementation of a control system designed for a laser cutting machine, primarily controlled using an ATmega32 microcontroller. The system facilitates precision cutting of various materials into predefined shapes such as lines, rectangles, circles, and sectors of circles, controlled via a user-friendly interface on a 4x4 matrix keypad.

## Features
- **Material Handling**: Supports cutting different materials in various shapes based on user input.
- **Memory Storage**: Utilizes EEPROM for storing up to 5 cutting item configurations.
- **Multi-axis Control**: Employs 2 stepper motors to maneuver the laser head along the X and Y axes.
- **User Interface**: Features a graphical 128x64 LCD display for real-time feedback and mode selection.
- **Multiple Modes**: Includes three operational modes for configuration and cutting execution:
  - **Mode 1**: Editing and viewing stored cutting items.
  - **Mode 2**: Adjusting motor speeds and table dimensions.
  - **Mode 3**: Executing the cutting process with visual feedback on the LCD.

## System Operation
The system's operation is divided into several key components:
1. **Mode Selection**: Users can switch between modes using the keypad.
2. **Item Configuration**: In Mode 1, users define the shape and parameters of the cutting items.
3. **System Settings**: Mode 2 allows setting the table dimensions and motor speeds.
4. **Execution**: Mode 3 activates the cutting process, where the machine follows the paths stored in EEPROM.

## Hardware Components
- **ATmega32 Microcontroller**: Manages all control logic and interface operations.
- **Stepper Motors**: Control the precise movement of the laser head.
- **ULN2003/L293 Drivers**: Drive the stepper motors based on control signals from the microcontroller.
- **4x4 Matrix Keypad**: Interface for user input.
- **128x64 LCD Display**: Provides a graphical interface for system status and mode management.
- **EEPROM**: Stores the cutting patterns and configurations.

## Disclaimer
This project is designed as a simulation and should not be used as a basis for actual cutting machine operations. We are not responsible for any damage to your equipment or any other related incidents that might occur if this simulation project is used beyond its intended educational purposes.

## Getting Started
To set up and run this project:
1. **Clone the repository**: 
   ```bash
   git clone [https://github.com/Arsemi4699/Laser-Cut-Machine.git]
2. **Go to Proteus directory and open it with Proteus8**
3. **test it**:
    use run option of Proteus; if didn't work, right click on AVR component and add project.hex file from Hex file directory
    
## Contributing
Contributions to this project are welcome. Please fork the repository and submit pull requests with your enhancements. For major changes, please open an issue first to discuss what you would like to change.
