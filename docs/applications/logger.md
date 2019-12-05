---
layout: default 
title: Logger
parent: Applications
nav_order: 7
---

# Logger

This app subscribes to critical software bus messages and logs them in a file.
Each type of message is logged in a separate file. The logs is created under
[cFS/bin/ram/IClog]() with a timestamp. These logs can be replayed to analyze
the flights.

By default, the logger application is set to log data. This behaviour can be
changed to a playback mode from the [logger tables]() by disabling logging and
providing the time stamp and location of the logged files.
