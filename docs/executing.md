---
layout: default
title: Execution
nav_order: 3
---

## Executing Icarous

The cFS core executive is launched by running the `core-cpu1` executable. This
is installed in cFS/bin/cpu1 after running `make cpu1-install` in the previous
section.

This cFS executable in turn launches the varous applications configured to
run in the cFS/bin/cpu1/cfe_core_cpu1.scr. The CMake build system in the
ICAROUS repository automatically generates a cfe_core_cpu1.scr file with the
appropriate apps that are configured to run. More information about specific
applications and their functions can be found in the applications section.

NOTE: On a linux operating system, cFS software bus makes use of os queues
(mqueues) to enable data exchange between apps. The msg_max parameter located
under /proc/sys/fs/mqueue/msg_max file should be set to a value greater than
250.

```
# running the cFS executive
./core-cpu1 -C 1 -I 0
```

The argument `C` denotes the cpu id, and `I` denotes the spacecraft id. For
more details about cFS command line arguments refer to the help 

```
./core-cpu1 --help
```

Often times, the os queues can get saturated when running ICAROUS multiple
times. We recommend removing any existing queues created by ICAROUS under
/dev/mqueue/ before launching ICAROUS. 

### Sample output from launching the cFS executive

A sample output from launching the core executive is shown below.
The output can differ based on the applications that are configured to startup.

```
CFE_PSP: CPU ID: 1
CFE_PSP: Spacecraft ID: 0
CFE_PSP: Default Reset Type = PO
CFE_PSP: Default Reset SubType = 1
CFE_PSP: Default CPU Name: cpu1
CFE_PSP: Starting the cFE with a POWER ON reset.
CFE_PSP: Clearing out CFE CDS Shared memory segment.
CFE_PSP: Clearing out CFE Reset Shared memory segment.
CFE_PSP: Clearing out CFE User Reserved Shared memory segment.
2019-192-17:19:16.99465 POWER ON RESET due to Power Cycle (Power Cycle).
2019-192-17:19:16.99482 ES Startup: CFE_ES_Main in EARLY_INIT state
CFE_PSP: CFE_PSP_AttachExceptions Called
2019-192-17:19:16.99497 ES Startup: CFE_ES_Main entering CORE_STARTUP state
2019-192-17:19:16.99498 ES Startup: Starting Object Creation calls.
2019-192-17:19:16.99498 ES Startup: Calling CFE_ES_CDSEarlyInit
2019-192-17:19:16.99515 ES Startup: Calling CFE_EVS_EarlyInit
2019-192-17:19:16.99541 Event Log cleared following power-on reset
2019-192-17:19:16.99542 ES Startup: Calling CFE_SB_EarlyInit
2019-192-17:19:16.99556 ES Startup: Calling CFE_TIME_EarlyInit
1970-012-14:03:20.00000 ES Startup: Calling CFE_TBL_EarlyInit
1970-012-14:03:20.00021 ES Startup: Calling CFE_FS_EarlyInit
1970-012-14:03:20.00035 ES Startup: Core App: CFE_EVS created. App ID: 0
EVS Port1 0/1/CFE_EVS 14: No subscribers for MsgId 0x808,sender CFE_EVS
EVS Port1 0/1/CFE_EVS 1: cFE EVS Initialized. cFE Version 6.5.0.0
1970-012-14:03:20.05147 ES Startup: Core App: CFE_SB created. App ID: 1
1970-012-14:03:20.05153 SB:Registered 4 events for filtering
EVS Port1 0/1/CFE_SB 14: No subscribers for MsgId 0x808,sender CFE_SB
EVS Port1 0/1/CFE_SB 1: cFE SB Initialized
1970-012-14:03:20.10174 ES Startup: Core App: CFE_ES created. App ID: 2
EVS Port1 0/1/CFE_SB 14: No subscribers for MsgId 0x808,sender CFE_ES
EVS Port1 0/1/CFE_ES 1: cFE ES Initialized
EVS Port1 0/1/CFE_SB 14: No subscribers for MsgId 0x808,sender CFE_ES
EVS Port1 0/1/CFE_ES 2: Versions:cFE 6.5.0.0, OSAL 4.2.1.0, PSP 1.3.0.0, chksm 65535
EVS Port1 0/1/CFE_SB 14: No subscribers for MsgId 0x808,sender CFE_ES
EVS Port1 0/1/CFE_ES 91: Mission b2765d9-dirty.Icarous, b2765d9-dirty, 7139592-dirty
EVS Port1 0/1/CFE_ES 92: Build 201907080938 sbalach2@LASLA18100675
1970-012-14:03:20.15282 ES Startup: Core App: CFE_TIME created. App ID: 3
EVS Port1 0/1/CFE_TIME 1: cFE TIME Initialized
1970-012-14:03:20.20392 ES Startup: Core App: CFE_TBL created. App ID: 4
EVS Port1 0/1/CFE_TBL 1: cFE TBL Initialized.  cFE Version 6.5.0.0
1970-012-14:03:20.25464 ES Startup: Finished ES CreateObject table entries.
1970-012-14:03:20.25466 ES Startup: CFE_ES_Main entering CORE_READY state
1970-012-14:03:20.25569 ES Startup: Opened ES App Startup file: /cf/cfe_es_startup.scr
1970-012-14:03:20.25613 ES Startup: Loading shared library: /cf/port_lib.so
PORT_LIB Initialized.  Version 1.0.0.0
1970-012-14:03:20.25783 ES Startup: Loading file: /cf/macsch.so, APP: MACSCH
1970-012-14:03:20.25916 ES Startup: MACSCH loaded and created
EVS Port1 0/1/MACSCH 1: Mac Scheduler Interface initialized. Version 1.0
1970-012-14:03:20.25932 ES Startup: Loading file: /cf/gsInterface.so, APP: GSINTERFACE
1970-012-14:03:20.26054 ES Startup: GSINTERFACE loaded and created
1970-012-14:03:20.26069 ES Startup: Loading file: /cf/rotorsim.so, APP: ROTORSIM
EVS Port1 0/1/CFE_SB 7: Duplicate Subscription,MsgId 0x818 on GSINTF_PIPE pipe,app GSINTERFACE
EVS Port1 0/1/GSINTERFACE 1: GS Interface initialized. Version 1.0
1970-012-14:03:20.27978 ES Startup: ROTORSIM loaded and created
1970-012-14:03:20.27990 ES Startup: Loading file: /cf/plexil.so, APP: PLEXIL
EVS Port1 0/1/ROTORSIM 0: Rotorsim App Initialized. Version 1.0
1970-012-14:03:20.30154 ES Startup: PLEXIL loaded and created
1970-012-14:03:20.30167 ES Startup: Loading file: /cf/geofence.so, APP: GEOFENCE
EVS Port1 0/1/PLEXIL 1: Plexil Initialized. Version 4.0
1970-012-14:03:20.30407 ES Startup: GEOFENCE loaded and created
1970-012-14:03:20.30418 ES Startup: Loading file: /cf/traffic.so, APP: TRAFFIC
EVS Port1 0/1/GEOFENCE 0: Geofence App Initialized. Version 1.0
1970-012-14:03:20.30618 ES Startup: TRAFFIC loaded and created
1970-012-14:03:20.30631 ES Startup: Loading file: /cf/trajectory.so, APP: TRAJECTORY
EVS Port1 0/1/TRAFFIC 0: TRAFFIC App Initialized. Version 1.0
1970-012-14:03:20.30987 ES Startup: TRAJECTORY loaded and created
1970-012-14:03:20.31001 ES Startup: Loading file: /cf/logger.so, APP: LOG
EVS Port1 0/1/TRAJECTORY 0: TRAJECTORY App Initialized. Version 1.0
1970-012-14:03:20.31064 ES Startup: LOG loaded and created
EVS Port1 0/1/LOG 0: Logging application initialized in logging mode. Version 1.0
1970-012-14:03:20.36099 ES Startup: CFE_ES_Main entering OPERATIONAL state
```
