_________________________________________________________________________________________

This is the README for the Elevator Project in MAS246
The main project file is the Heis_prosjekt_2025.ino


Group 9    


Authors:
- Oscar Fredrik Baldetorp
- Haakon Huglen
- Tobias Norheim
__________________________________________________________________________________________

Overview
--------

This project implements a complete elevator control system on the AIOSL (All-in-One Servo Lab) platform.
The software is written in Arduino/C++ and uses a modular design consisting of custom classes for motor control, encoder handling, display management, button interfaces, queue logic, and safety functions.

The system controls:
- A DC motor for vertical motion
- A stepper motor for opening and closing the doors
- A quadrature encoder for precise position feedback
- Inside elevator and hall buttons
- LCD display
- LEDs for floor indication
- Overweight and emergency-stop logic
- A SCAN-based queue management system for scheduling floor requests


Uploading the Code
------------------

1. Open Heis_prosjekt_2025.ino in Arduino IDE.
2. Connect the AISOL unit via USB.
3. Select the correct board (Arduino Mega 2560).
4. Upload the program.

All project files must be kept in the same folder for Arduino IDE to detect them.


Modifying the Code
------------------

PID Settings:
PID parameters for the DC motor are defined in DC_motor.cpp:
- Kp, Ki, and Kd are initialized in the constructor.

Tolerance:
Allowed position tolerance is defined in Config.h:
const long POS_TOLERANCE = 8.25; //ticks

Starting Floor:
Defined in the Elevator class constructor:
_current_floor = MIN_FLOOR;

Door Parameters:
Adjustable in Step_motor.cpp.

Motor Speed:
Maximum PWM is set in:
_dc.motor_speed_max = 255;


Using the System
----------------

Power On:
After uploading the program, turn on the AISOL unit’s power.

Floor Selection:
- Inside buttons select floors 1–4.
- Hall buttons create up/down requests to floor -
(U need to use serial monitor wth 15200 baud rate. To call the elveator u need to type what floor you are in, and if your going down or up).
- Requests are processed using SCAN-based scheduling.

LCD Interface:
Shows:
- Current floor
- Direction (Up/Down)
- Emergency stop
- Overweight warnings
- Door states

LED Indicators:
Show the active floor.

Emergency Stop:
- Press once: elevator stops immediately.
- Press again: elevator resumes when safe.

Overweight Sensor:
Triggers if analog input exceeds threshold:
- Elevator stops
- Displays “Overweight”
- Resumes when weight is below threshold

Door Operation:
While the stepper motor opens or closes the door, no new requests are processed.

Queue Testing:
To test queue logic without motion:
- Power AISOL using USB and the main power supply
- Turn off the motor power switch
- Logic, buttons, LCD, and LEDs remain active
- Requests can be queued
- Re-enable motor power to execute queued requests


__________________________________________________________________________________________
