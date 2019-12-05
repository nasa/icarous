---
layout: default 
title: GsInterface
parent: Applications
nav_order: 2
---

# Ground-station interface

This is a cFS application that serves as a bridge between ICAROUS and a mavlink
compatible ground control station (GCS). The connection to the ground station
can be established via a serial connection (SERIAL) or a socket connection
(SOCKET). The connection parameters are configured in the [gsIntf_tbl.c]()
file. Software bus messages are converted into appropriate mavlink messages and
sent to the GCS.

This app also implements the mavlink parameter protocol to handle parameter
updates from the GCS. The received parameters are then converted into
appropriate software bus messages and published so that other apps can update
their parameters. Furthermore, this app also enables ICAROUS to receive flightplans,
geofences and commands.

NOTE: The app must be recompiled after making changes to the values in gsIntf_tbl.c table.

## Parameters found in gsIntf_tbl.c
- **PortType** : SERIAL (for serial connections) / SOCKET (for socket connection)
- **BaudRate** : Baud rate of serial connection (only used for SERIAL)
- **Portin** : (For SOCKET) listen to incoming data on this port.
- **Portout** : (For SOCKET) Output data to this port. Set this to 0 and the app will automatically configure the output port number based on incoming data packets.
- **Address** : String specifying IP address (for SOCKET) or serail port name (for SERIAL).