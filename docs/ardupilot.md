---
layout: default 
title: ardupilot
nav_order: 1
parent: applications
---

The ardupilot app servers as an interface between the ardupilot autopilot system and the other functionalities in Icarous.

The ardupilot app received mavlink messages from the ardupilot and translates them into appropriate cFS software bus messages and vice versa. Position, attitude, heartbeat messages are extracted.

Commands from the cFS software bus are translated into appropriate mavlink commands and sent to the autopilot. Icarous switches to active mode before taking control.

The app supports either a serial connection or a socket connection to the auopilot. Using the socket interface, this app also helps interface with the ardupilot sitl.




