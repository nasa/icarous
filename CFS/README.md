ICAROUS-CFS implementation
============================

We provide several CFS apps that run on the CFS middleware that collectively provide the ICAROUS functionality. The CFS apps communicate using the CFS software bus executive service.

### CFS Version

- This implementation uses cFE release 6.5.0 with OSAL version 4.2.0.0.
- Visit (https://cfs.gsfc.nasa.gov/) for more information on CFS, OSAL and other relevant apps.

### ICAROUS apps

- interface: This is an app to interface ICAROUS with a mavlink compatible autopilot. Use this app as an example if you want to build an interface to other non-mavlink autopilots.
- icarous: This app initializes ICAROUS and handles sending/receiving commands to/from the core ICAROUS application.
- icarouslib: The core C++ Icarous library.

### Configuring ICAROUS
- Use the table services to configure user define parameters.
- The interface table can be found in apps/interface/fsw/tables.
- The interface table defines the UDP host,port numbers and serail port configurations.
- The icarous table can be found in apps/icarous/fsw/tables
- The icarous table defines the icarous configurations.
- The table binaries will have to be regenerated. Use make as follows to build the app tables:
```
$ make cfs_build_app_tbls
```
### Compiling and running the ICAROUS app suite

- From the CFS/ folder, setup the environment variables
```
 $ source setvars.sh
```
- Compile CFS with the relevant apps. This single command takes care of everything.
```
$ cd build/cpu1
$ make compile
```
- Start the app. You may need to use sudo.
```
$ cd exe
$ sudo ./core-linux.bin
```
