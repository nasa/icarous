---
layout: default 
title: Arducopter
parent: Applications
nav_order: 1
---

# Arducopter interface

This is a cFS application that serves as a bridge between ICAROUS and the [arducopter](http://ardupilot.org/) autopilot. The connection to the autopilot can be established via a serial connection (SERIAL) or a socket connection (SOCKET). The connection parameters are configured in the [intf_tbl.c]() file. MAVLink messages from the autopilot device are converted into appropriate software bus messages.

Exploiting the use of [mavlink routing]() implemented within the Arducopter flight stack, this app sends ICAROUS related telemtry (e.g. ICAROUS sense avoid data, reroute trajectories) to the ground station via the Arducopter's main telemetry link. See [setup]() for more information on how to setup an additional ground control station to upload data to ICAROUS (i.e. geofences, parameters and commands). 

Although commands to ICAROUS (e.g. start,reset) can be send to ICAROUS via the additional ground control station discussed above, it is sometimes desirable to configure switches (channels) on a hand held r/c transmitter to engage and disengage ICAROUS. ICAROUS can be engagned/disengaged using a single channel on the r/c transmitter by configuring appropriate channel numbers and PWM signal values in [intf_tbl.c](). Note: Internally the app looks for pwm signal within an interval (+/- 300) of the specified value in intf_tbl.c. 

The app has been tested on arducopter 3.6

NOTE: The app must be recompiled after making changes to the values in intf_tbl.c table.

## Parameters found in intf_tbl.c
- **PortType** : SERIAL (for serial connections) / SOCKET (for socket connection)
- **BaudRate** : Baud rate of serial connection (only used for SERIAL)
- **Portin** : (For SOCKET) listen to incoming data on this port.
- **Portout** : (For SOCKET) Output data to this port. Set this to 0 and the app will automatically configure the output port number based on incoming data packets.
- **Address** : String specifying IP address (for SOCKET) or serail port name (for SERIAL).
- **icRcChannel** : Channel on r/c transmitter to send start/reset signals to Icarous.
- **pwmStart** : A PWM signal value within [pwmStart-300,pwmStart+300] will be recognized as a start command.
- **pwmReset** : A PWM signal value within [pwmStop-300,pwmStop+300] will be recognized as a reset command.
