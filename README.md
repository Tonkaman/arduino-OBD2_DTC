# arduino-OBD2_DTCs
A library for converting OBDII Diagnostic Trouble Codes (DTC) to human readable text

This library is used to convert a DTC in the standard European On-Board Diagnostics (EOBD) format to a human readable text description.  This format is a letter, followed by four numbers.  The letter and first two numbers correspond to a specific vehicle subsystem.  The last two numbers refer to the individual fault code within the subsystem.

* P00xx - Fuel and Air Metering and Auxiliary Emission Controls
* P01xx - Fuel and Air Metering
* P02xx - Fuel and Air Metering (Injector Circuit)
* P03xx - Ignition System or Misfire
* P04xx - Auxiliary Emissions Controls
* P05xx - Vehicle Speed Controls and Idle Control System
* P06xx - Computer Output Circuit
* P07xx - Transmission
* P08xx - Transmission
* P09xx - Transmission
* C00xx - Chassis (includes ABS faults)
* B00xx - Body (includes climate control and airbag faults)
* U00xx - Network (refers to faults within the CAN bus system)

##DTC Definitions
All trouble code definitions and descriptions were pulled from SAE Standard J2021, last revised December 13, 2016.

## Library Usage
To save on program memory when using AVR microcontrollers, the majority of these DTCs are commented out within the .cpp file.  If the fault you need is not being returned, check if it is in one of the blocks commented out.
